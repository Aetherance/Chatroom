#include"client.h"
#include<iomanip>
#include<vector>
#include <chrono>
#include"Timestamp.h"

using namespace ftxui;

extern std::unordered_map<std::string,std::vector<messageinfo>> messageMap;

extern std::unordered_map<std::string,bool> NewMessageMap;

extern bool isGroupBreak;

extern std::string show_info;

extern std::string show_info2;

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
Component setting_btn;

std::string wrapTextSmart(const std::string& text, size_t lineLength);

std::vector<std::string> splitLines(const std::string& input);

void Client::MsgController() {
  // 输入组件及配置
  visible_lines = Terminal::Size().dimy - 13;
  MessageKey = msgClient_.peerEmail();
  PeerUserName = msgClient_.peerUsername();
  PeerEmail = msgClient_.peerEmail();

  NewMessageMap[msgClient_.peerEmail()] = false;

  msgClient_.pullDownloadList(msgClient_.LocalEmail(),msgClient_.peerEmail());

  if(msgClient_.isPeerGroup()) {
    pullGroupMembers();
    msgClient_.pullGroupOwner(false,msgClient_.peerEmail());
    msgClient_.pullGroupOPs(false,msgClient_.peerEmail());
  }

  auto input_option = InputOption();

  input_option.on_enter = [&,this] {
    input_content = wrapTextSmart(input_content,Terminal::Size().dimx - 5);
    visible_lines = Terminal::Size().dimy - 13;
    if (!input_content.empty()) {   
      if(parseCommand(input_content)) {
        return;
      }

      if(input_content == "\n") {
        input_content.clear();

        return;
      }

      std::vector<std::string> lines = splitLines(input_content);

      for(int i = 0;i<lines.size();i++) {
        messageMap[MessageKey].push_back({i ? "" : "You", lines[i],ilib::base::Timestamp::now().microSecondsSinceEpoch()});
      }

      msgClient_.sendMsgPeer(input_content);
      
      input_content.clear();
      
      MsgScreenScrollOffset[msgClient_.peerEmail()] = std::max(0, static_cast<int>(messageMap[MessageKey].size()) - visible_lines);
    }
  };

  input = Input(&input_content, "输入消息", input_option);

  // 发送按钮
  send_btn = Button("发送", [&] {
    input_content = wrapTextSmart(input_content,Terminal::Size().dimx - 5);
    if (!input_content.empty()) {
      
      if(parseCommand(input_content)) {
        return;
      }

      std::vector<std::string> lines = splitLines(input_content);

      for(int i = 0;i<lines.size();i++) {
        messageMap[MessageKey].push_back({i ? "" : "You", lines[i],ilib::base::Timestamp::now().microSecondsSinceEpoch()});
      }

      msgClient_.sendMsgPeer(input_content);
      
      input_content.clear();
      // 新消息自动滚动到底部
      MsgScreenScrollOffset[msgClient_.peerEmail()] = std::max(0, static_cast<int>(messageMap[MessageKey].size()) - visible_lines);
    }
  });

  file_btn = Button("文件...", [&] {
    fileService();
  });

  setting_btn = Button("设置...",[&] {
    if(msgClient_.isPeerGroup()) {
      GroupSettings();
      if(isGroupBreak) {
        MsgScreen.Exit();
      }
    } else {
      UserSettings();
    }
  });

  // 初始位置在底部
  MsgScreenScrollOffset[msgClient_.peerEmail()] = std::max(0, static_cast<int>(messageMap[MessageKey].size()) - visible_lines);

  auto renderer = makeRenderer();
  // 运行界面
  MsgScreen.Loop(renderer);
}

Element Client::makeSidebar() {
  // 使用局部变量确保不修改原始数据
  const auto members = msgClient_.getGroupMembers(msgClient_.peerEmail());
  static auto lastMembers = members;  // 静态变量用于比较变化

  if(msgClient_.isPeerGroup()) {
    // 检查成员列表是否变化
    bool membersChanged = (lastMembers != members);
    lastMembers = members;  // 更新缓存
    
    Elements sidebarText;
    for(const auto& entry : members) {  // 使用const避免修改
      if(entry.empty()) continue;  // 安全处理空条目
      
      // 安全提取成员信息和等级
      const char memberLevel = entry.back();
      const std::string memberName = entry.substr(0, entry.size() - 1);
      
      // 创建元素
      Element text_show;
      if(memberLevel == GROUP_OWNER[0]) {
        text_show = text(memberName + " (群主)") | bold | color(Color::DarkGoldenrod);
      } else if(memberLevel == GROUP_OP[0]) {
        text_show = text(memberName + " 🛡️") | color(Color::CyanLight);
      } else {
        text_show = text(memberName);
      }
      sidebarText.push_back(text_show);
    }
    
    // 返回群成员面板
    return vbox({
      text("群成员") | bold | center,
      separator(),
      vbox(std::move(sidebarText))  // 移动语义优化
        | flex | vscroll_indicator | frame
    }) | border;
  } else {
    return vbox();  // 非群组时返回空
  }
}

Component Client::makeRenderer() {
  auto layout = Container::Horizontal({input, send_btn , file_btn,setting_btn});
  
  return 
  Renderer(layout, [&] {
    // 计算可见的消息范围
    int start = std::max(0, MsgScreenScrollOffset[msgClient_.peerEmail()]);
    int end = std::min(static_cast<int>(messageMap[MessageKey].size()), start + visible_lines);

    Elements visible_elements;
    for (int i = start; i < end; ++i) {
      if(messageMap[MessageKey][i].from == "You" || messageMap[MessageKey][i].from == "") {
        visible_elements.push_back(text(getTime(messageMap[MessageKey][i].timestamp) + messageMap[MessageKey][i].from + ": " + messageMap[MessageKey][i].text) | color(Color::GrayLight));
      } else {
        visible_elements.push_back(text(getTime(messageMap[MessageKey][i].timestamp) + messageMap[MessageKey][i].from + ": " + messageMap[MessageKey][i].text));
      }
    }

    // 基本边栏框架
    auto sidebar = makeSidebar();
    
    return vbox(
      hbox({
        text(" 💬  " + show_info)
      }) | size(HEIGHT,EQUAL, show_info.empty() ? 0 : 1) | (show_info.empty() ? size(WIDTH,EQUAL,0) : border),
      hbox({
        text(" 💬 " + show_info2)
      }) | size(HEIGHT,EQUAL, show_info2.empty() ? 0 : 1) | (show_info2.empty() ? size(WIDTH,EQUAL,0) : border),
      // 主聊天区域
      hbox({
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
            file_btn->Render(),
            setting_btn->Render()
          }) | border
        }) | flex,
        
        // 右侧边栏
        sidebar
      }) | border | flex
    );
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

bool isPureAscii(const std::string& str) {
    for (char c : str) {
        if (static_cast<unsigned char>(c) >= 128) {
            return false;
        }
    }
    return true;
}

// ASCII字符串的智能换行
std::string wrapAsciiText(const std::string& text, size_t lineLength) {
    if (lineLength == 0 || text.empty()) return text;
    
    std::string result;
    size_t lineStart = 0;
    const size_t textLen = text.length();
    
    while (lineStart < textLen) {
        size_t lineEnd = lineStart + lineLength;
        
        // 处理剩余文本不足一行的情况
        if (lineEnd >= textLen) {
            result += text.substr(lineStart);
            break;
        }
        
        // 查找最近的换行点（空格或标点）
        size_t wrapPos = lineEnd;
        bool foundBreak = false;
        
        // 优先在空格处换行
        size_t lastSpace = text.find_last_of(" \t", lineEnd);
        if (lastSpace != std::string::npos && lastSpace > lineStart) {
            wrapPos = lastSpace;
            foundBreak = true;
        }
        
        // 其次在标点处换行
        if (!foundBreak) {
            for (size_t i = lineEnd; i > lineStart; --i) {
                if (ispunct(static_cast<unsigned char>(text[i]))) {
                    wrapPos = i + 1;
                    foundBreak = true;
                    break;
                }
            }
        }
        
        // 添加当前行
        result += text.substr(lineStart, wrapPos - lineStart) + '\n';
        
        // 移动到下一行起始位置（跳过空格）
        lineStart = wrapPos;
        while (lineStart < textLen && isspace(static_cast<unsigned char>(text[lineStart]))) {
            lineStart++;
        }
    }
    
    return result;
}

// 检查字节是否是UTF-8字符的起始字节
inline bool isUtf8StartByte(unsigned char c) {
    return (c & 0xC0) != 0x80;
}

// 查找前一个UTF-8字符的起始位置
size_t findPrevUtf8Char(const std::string& str, size_t pos) {
    while (pos > 0 && !isUtf8StartByte(static_cast<unsigned char>(str[pos]))) {
        --pos;
    }
    return pos;
}

// 非ASCII字符串的智能换行（UTF-8）
std::string wrapUtf8Text(const std::string& text, size_t charLength) {
    if (charLength == 0 || text.empty()) return text;
    
    std::string result;
    size_t lineStart = 0;
    const size_t byteLen = text.length();
    
    while (lineStart < byteLen) {
        size_t byteEnd = lineStart;
        size_t charCount = 0;
        
        // 计算当前行字符数
        while (byteEnd < byteLen && charCount < charLength) {
            size_t charBytes = 1;
            unsigned char c = static_cast<unsigned char>(text[byteEnd]);
            
            // 确定UTF-8字符长度
            if (c >= 0xF0) charBytes = 4;       // 4字节字符
            else if (c >= 0xE0) charBytes = 3;  // 3字节字符
            else if (c >= 0xC0) charBytes = 2;  // 2字节字符
            
            // 检查字符是否完整
            if (byteEnd + charBytes > byteLen) {
                break; // 遇到不完整字符
            }
            
            byteEnd += charBytes;
            charCount++;
        }
        
        // 处理剩余文本不足一行的情况
        if (byteEnd >= byteLen) {
            result += text.substr(lineStart);
            break;
        }
        
        // 查找最近的换行点（优先在字符边界）
        size_t wrapPos = byteEnd;
        
        // 查找最近的空格
        size_t lastSpace = text.find_last_of(" \t", byteEnd);
        if (lastSpace != std::string::npos && lastSpace > lineStart) {
            wrapPos = lastSpace;
        } 
        // 没有空格，确保在字符边界换行
        else {
            wrapPos = findPrevUtf8Char(text, byteEnd);
        }
        
        // 添加当前行
        result += text.substr(lineStart, wrapPos - lineStart) + '\n';
        
        // 移动到下一行起始位置（跳过空格）
        lineStart = wrapPos;
        while (lineStart < byteLen && isspace(static_cast<unsigned char>(text[lineStart]))) {
            lineStart++;
        }
    }
    
    return result;
}

// 主换行函数：自动区分ASCII和非ASCII
std::string wrapTextSmart(const std::string& text, size_t lineLength) {
    if (isPureAscii(text)) {
        return wrapAsciiText(text, lineLength);
    } else {
        return wrapUtf8Text(text, lineLength);
    }
}

std::vector<std::string> splitLines(const std::string& input) {
    std::vector<std::string> lines;
    if (input.empty()) return lines;  // 处理空输入
    
    size_t start = 0;
    size_t end = input.find('\n');
    
    while (end != std::string::npos) {
        // 从start到end（不包括换行符）的子串
        lines.push_back(input.substr(start, end - start));
        
        // 移动到下一行的开始（跳过换行符）
        start = end + 1;
        end = input.find('\n', start);
    }
    
    // 添加最后一行（没有换行符结尾的部分）
    if (start < input.length()) {
        lines.push_back(input.substr(start));
    }
    
    return lines;
}
