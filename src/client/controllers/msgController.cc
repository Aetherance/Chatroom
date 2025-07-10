#include"client.h"
#include<iomanip>
#include<vector>
#include <chrono>
#include"Timestamp.h"

using namespace ftxui;

extern std::unordered_map<std::string,std::vector<messageinfo>> messageMap;

extern std::unordered_map<std::string,bool> NewMessageMap;

std::string MessageKey;
std::string PeerUserName;
std::string PeerEmail;

std::string getTime(int64_t rawtime);

ScreenInteractive MsgScreen = ScreenInteractive::Fullscreen();
  // 状态变量
std::unordered_map<std::string,int> MsgScreenScrollOffset;

int visible_lines = Terminal::Size().dimy - 13;
std::string input_content;

Component input;
Component send_btn;
Component file_btn;

void Client::MsgController() {
  // 输入组件及配置
  MessageKey = msgClient_.peerEmail();
  PeerUserName = msgClient_.peerUsername();
  PeerEmail = msgClient_.peerEmail();

  NewMessageMap[msgClient_.peerEmail()] = false;

  msgClient_.pullDownloadList(msgClient_.LocalEmail(),msgClient_.peerEmail());

  auto input_option = InputOption();
  input_option.on_enter = [&,this] {
    if (!input_content.empty()) {   
      if(parseCommand(input_content)) {
        return;
      }

      if(input_content == "\n") {
        input_content.clear();

        return;
      }

      messageMap[MessageKey].push_back({"You", input_content,ilib::base::Timestamp::now().microSecondsSinceEpoch()});

      msgClient_.sendMsgPeer(input_content);
      
      input_content.clear();
      
      MsgScreenScrollOffset[msgClient_.peerEmail()] = std::max(0, static_cast<int>(messageMap[MessageKey].size()) - visible_lines);
    }
  };

  input = Input(&input_content, "输入消息", input_option);

  // 发送按钮
  send_btn = Button("发送", [&] {
    if (!input_content.empty()) {
      
      if(parseCommand(input_content)) {
        return;
      }

      messageMap[msgClient_.peerEmail()].push_back({"You", input_content, ilib::base::Timestamp::now().microSecondsSinceEpoch()});

      msgClient_.sendMsgPeer(input_content);
      
      input_content.clear();
      // 新消息自动滚动到底部
      MsgScreenScrollOffset[msgClient_.peerEmail()] = std::max(0, static_cast<int>(messageMap[MessageKey].size()) - visible_lines);
    }
  });

  file_btn = Button("文件...", [&] {
    fileService();
  });

  // 初始位置在底部
  MsgScreenScrollOffset[msgClient_.peerEmail()] = std::max(0, static_cast<int>(messageMap[MessageKey].size()) - visible_lines);

  auto renderer = makeRenderer();
  // 运行界面
  MsgScreen.Loop(renderer);
}

Element Client::makeSidebar() {
  Elements sidebarText;
  auto members = msgClient_.getGroupMembers(msgClient_.peerEmail());

  if(msgClient_.isPeerGroup()) {
    for(auto & entry : members) {
      Element text_show;
      auto memberLevel = entry[entry.size()-1];
      entry.resize(entry.size() - 1);
      if(memberLevel == GROUP_OWNER[0]) {
        text_show = text(entry + " (群主)") | bold | color(Color::DarkGoldenrod);
      } else if(memberLevel == GROUP_OP[0]) {
        text_show = text(entry + " 🛡️") | color(Color::CyanLight);
      } else {
        text_show = text(entry);
      }
      sidebarText.push_back(text_show);
    }
    return vbox({
      text("群成员") | bold | center,
      separator(),
      vbox({vbox(sidebarText) | flex | center})
        | vscroll_indicator 
        | frame 
        | flex
    }) 
    | border;
  } else {
    return vbox();
  }
}

Component Client::makeRenderer() {
  auto layout = Container::Horizontal({input, send_btn , file_btn});
  
  return 
  Renderer(layout, [&] {
    // 计算可见的消息范围
    int start = std::max(0, MsgScreenScrollOffset[msgClient_.peerEmail()]);
    int end = std::min(static_cast<int>(messageMap[MessageKey].size()), start + visible_lines);

    Elements visible_elements;
    for (int i = start; i < end; ++i) {
      visible_elements.push_back(text(getTime(messageMap[MessageKey][i].timestamp) + messageMap[MessageKey][i].from + ": " + messageMap[MessageKey][i].text));
    }

    // 基本边栏框架
    auto sidebar = makeSidebar();

    return hbox({
      // 主聊天区域
      vbox({
        text(PeerUserName) | bold | center,
        text(PeerEmail) | center,
        separator(),
        vbox(visible_elements)  // 只渲染可见部分
          | vscroll_indicator 
          | frame 
          | yflex 
          | flex_shrink 
          | border,
        separator(),
        hbox({
          input->Render() | flex,
          send_btn->Render(),
          file_btn->Render()
        }) | border
      }) | flex,
      
      // 右侧边栏
      sidebar
    }) | border | flex;
  }) | CatchEvent([&](Event event) -> bool {
    if (event == Event::Escape) {
      MsgScreen.Exit();
      return true;
    }
    
    // 键盘滚动控制
    if (event == Event::ArrowUp) {
      MsgScreenScrollOffset[msgClient_.peerEmail()] = std::max(0, MsgScreenScrollOffset[msgClient_.peerEmail()] - 1);
      return true;
    }
    if (event == Event::ArrowDown) {
      MsgScreenScrollOffset[msgClient_.peerEmail()] = std::min(static_cast<int>(messageMap[MessageKey].size()) - visible_lines, MsgScreenScrollOffset[msgClient_.peerEmail()] + 1);
      return true;
    }
    if (event == Event::PageUp) {
      MsgScreenScrollOffset[msgClient_.peerEmail()] = std::max(0, MsgScreenScrollOffset[msgClient_.peerEmail()] - visible_lines);
      return true;
    }
    if (event == Event::PageDown) {
      MsgScreenScrollOffset[msgClient_.peerEmail()] = std::min(static_cast<int>(messageMap[MessageKey].size()) - visible_lines, MsgScreenScrollOffset[msgClient_.peerEmail()] + visible_lines);
      return true;
    }
    
    // 鼠标滚轮控制
    if (event.is_mouse()) {
      auto& mouse = event.mouse();
      if (mouse.button == Mouse::WheelUp) {
        MsgScreenScrollOffset[msgClient_.peerEmail()] = std::max(0, MsgScreenScrollOffset[msgClient_.peerEmail()] - 2);  // 滚轮加速滚动
        return true;
      }
      if (mouse.button == Mouse::WheelDown) {
        MsgScreenScrollOffset[msgClient_.peerEmail()] = std::min(static_cast<int>(messageMap[MessageKey].size()) - visible_lines, MsgScreenScrollOffset[msgClient_.peerEmail()] + 2);
        return true;
      }
    }
    
    return false;
  }) | color(Color::White) | bgcolor(Color::RGB(22, 22, 30));
}

std::string getTime(int64_t microSecondsSinceEpoch) {
  using namespace std::chrono;

  auto timePoint = time_point<system_clock>(microseconds(microSecondsSinceEpoch));

  time_t time = system_clock::to_time_t(timePoint);

  struct tm localTime;
 
  if (localtime_r(&time, &localTime) == nullptr) {
      return "[Invalid Time] ";
  }
  
  std::stringstream ss;
  ss << std::put_time(&localTime, "%H:%M");
  return "[" + ss.str() + "] ";
}