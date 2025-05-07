#include"client.h"
#include<iomanip>
#include<vector>
#include <chrono>

using namespace ftxui;

void Client::MsgController() {
  // 状态变量
  std::vector<std::string> messages;
  std::string input_content;

  auto screen = ScreenInteractive::Fullscreen();

  // 输入组件及配置
  auto input_option = InputOption();
  input_option.on_enter = [&] {
    if (!input_content.empty()) {
      messages.push_back("You: " + input_content);
      input_content.clear();
    }
  };
  auto input = Input(&input_content, "输入消息", input_option);

  // 发送按钮
  auto send_btn = Button("发送", [&] {
    if (!input_content.empty()) {
      messages.push_back("You: " + input_content);
      input_content.clear();
    }
  });

  // 组合布局
  auto layout = Container::Horizontal({input, send_btn});

  // 主渲染逻辑
  auto renderer = Renderer(layout, [&] {
    Elements chat_lines;
    for (auto& msg : messages) {
      chat_lines.push_back(text(msg));
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
      | size(WIDTH, EQUAL, 16); // 固定宽度25字符

    return hbox({
      // 主聊天区域
      vbox({
        text("Chat with ") | bold | center,
        separator(),
        vbox(chat_lines) 
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
    if(event == Event::Escape) {
      screen.Exit();
      return true;
    } else {
      return false;
    }
  });

  // 填充测试数据
  for (int i = 0; i < 50; ++i) {
    messages.push_back("System: 测试消息 " + std::to_string(i+1));
  }

  // 运行界面
  screen.Loop(renderer);
}