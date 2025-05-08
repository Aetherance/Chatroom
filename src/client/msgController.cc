#include"client.h"
#include<iomanip>
#include<vector>
#include <chrono>

using namespace ftxui;

void Client::MsgController() {
  auto screen = ScreenInteractive::Fullscreen();
  // 状态变量
  std::vector<std::string> messages;
  int scroll_offset = 0;       // 当前滚动位置
  int visible_lines = Terminal::Size().dimy - 12;
  std::string input_content;


  // 输入组件及配置
  auto input_option = InputOption();
  input_option.on_enter = [&] {
    if (!input_content.empty()) {
      messages.push_back("You: " + input_content);
      input_content.clear();
      // 新消息自动滚动到底部
      scroll_offset = std::max(0, static_cast<int>(messages.size()) - visible_lines);
    }
  };
  auto input = Input(&input_content, "输入消息", input_option);

  // 发送按钮
  auto send_btn = Button("发送", [&] {
    if (!input_content.empty()) {
      messages.push_back("You: " + input_content);
      input_content.clear();
      // 新消息自动滚动到底部
      scroll_offset = std::max(0, static_cast<int>(messages.size()) - visible_lines);
    }
  });

  // 组合布局
  auto layout = Container::Horizontal({input, send_btn});

  // 主渲染逻辑
  auto renderer = Renderer(layout, [&] {
    // 计算可见的消息范围
    int start = std::max(0, scroll_offset);
    int end = std::min(static_cast<int>(messages.size()), start + visible_lines);

    Elements visible_elements;
    for (int i = start; i < end; ++i) {
      visible_elements.push_back(text(messages[i]));
    }

    // 基本边栏框架
    auto sidebar = vbox({
        text("Sidebar") | bold | center,
        separator(),
        vbox({text("Content Area") | center}) 
          | vscroll_indicator 
          | frame 
          | flex
      }) 
      | border 
      | size(WIDTH, EQUAL, 16);

    return hbox({
      // 主聊天区域
      vbox({
        text(msgClient_.peerUsername()) | bold | center,
        text(msgClient_.peerEmail()) | center,
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
      screen.Exit();
      return true;
    }
    
    // 键盘滚动控制
    if (event == Event::ArrowUp) {
      scroll_offset = std::max(0, scroll_offset - 1);
      return true;
    }
    if (event == Event::ArrowDown) {
      scroll_offset = std::min(static_cast<int>(messages.size()) - visible_lines, scroll_offset + 1);
      return true;
    }
    if (event == Event::PageUp) {
      scroll_offset = std::max(0, scroll_offset - visible_lines);
      return true;
    }
    if (event == Event::PageDown) {
      scroll_offset = std::min(static_cast<int>(messages.size()) - visible_lines, scroll_offset + visible_lines);
      return true;
    }
    
    // 鼠标滚轮控制
    if (event.is_mouse()) {
      auto& mouse = event.mouse();
      if (mouse.button == Mouse::WheelUp) {
        scroll_offset = std::max(0, scroll_offset - 2);  // 滚轮加速滚动
        return true;
      }
      if (mouse.button == Mouse::WheelDown) {
        scroll_offset = std::min(static_cast<int>(messages.size()) - visible_lines, scroll_offset + 2);
        return true;
      }
    }
    
    return false;
  });

  // 初始化测试消息
  for(int i = 0; i < 100; i++) {
    messages.push_back("msg test" + std::to_string(i + 1));
  }
  // 初始位置在底部
  scroll_offset = std::max(0, static_cast<int>(messages.size()) - visible_lines);

  // 运行界面
  screen.Loop(renderer);
}