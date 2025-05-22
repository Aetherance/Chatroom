#include"client.h"
#include<iomanip>
#include<vector>
#include <chrono>
#include"Timestamp.h"

using namespace ftxui;

extern std::unordered_map<std::string,std::vector<messageinfo>> messageMap;

std::string MessageKey;
std::string PeerUserName;
std::string PeerEmail;

std::string getTime(int64_t rawtime);

ScreenInteractive MsgScreen = ScreenInteractive::Fullscreen();
  // 状态变量
int MsgScreenScrollOffset = 0;  // 当前滚动位置
int visible_lines = Terminal::Size().dimy - 13;
std::string input_content;

Component input;
Component send_btn;

Element makeSidebar();
Component makeRenderer();

void Client::MsgController() {
  // 输入组件及配置
  MessageKey = msgClient_.peerEmail();
  PeerUserName = msgClient_.peerUsername();
  PeerEmail = msgClient_.peerEmail();

  auto input_option = InputOption();
  input_option.on_enter = [&] {
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
      
      MsgScreenScrollOffset = std::max(0, static_cast<int>(messageMap[MessageKey].size()) - visible_lines);
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
      MsgScreenScrollOffset = std::max(0, static_cast<int>(messageMap[MessageKey].size()) - visible_lines);
    }
  });

  // 初始位置在底部
  MsgScreenScrollOffset = std::max(0, static_cast<int>(messageMap[MessageKey].size()) - visible_lines);

  auto renderer = makeRenderer();
  // 运行界面
  MsgScreen.Loop(renderer);
}


Element makeSidebar() {
  return vbox({
    text("Sidebar") | bold | center,
    separator(),
    vbox({text("Content Area") | center}) 
      | vscroll_indicator 
      | frame 
      | flex
  }) 
  | border 
  | size(WIDTH, EQUAL, 16);
}

Component makeRenderer() {
  auto layout = Container::Horizontal({input, send_btn});
  
  return 
  Renderer(layout, [&] {
    // 计算可见的消息范围
    int start = std::max(0, MsgScreenScrollOffset);
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
          send_btn->Render()
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
      MsgScreenScrollOffset = std::max(0, MsgScreenScrollOffset - 1);
      return true;
    }
    if (event == Event::ArrowDown) {
      MsgScreenScrollOffset = std::min(static_cast<int>(messageMap[MessageKey].size()) - visible_lines, MsgScreenScrollOffset + 1);
      return true;
    }
    if (event == Event::PageUp) {
      MsgScreenScrollOffset = std::max(0, MsgScreenScrollOffset - visible_lines);
      return true;
    }
    if (event == Event::PageDown) {
      MsgScreenScrollOffset = std::min(static_cast<int>(messageMap[MessageKey].size()) - visible_lines, MsgScreenScrollOffset + visible_lines);
      return true;
    }
    
    // 鼠标滚轮控制
    if (event.is_mouse()) {
      auto& mouse = event.mouse();
      if (mouse.button == Mouse::WheelUp) {
        MsgScreenScrollOffset = std::max(0, MsgScreenScrollOffset - 2);  // 滚轮加速滚动
        return true;
      }
      if (mouse.button == Mouse::WheelDown) {
        MsgScreenScrollOffset = std::min(static_cast<int>(messageMap[MessageKey].size()) - visible_lines, MsgScreenScrollOffset + 2);
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