#include "client.h"

using namespace ftxui;

ScreenInteractive fileTranScreen = ScreenInteractive::Fullscreen();

void Client::fileTrans() {
  
  auto renderer = Renderer([&] {
    // 创建文件列表元素
    Elements file_entries;
    for (auto& file : ftpClient_.transList) {
      file_entries.push_back(
        hbox({
          text(file.first) | flex | size(WIDTH, EQUAL, 16),
          gauge(ftpClient_.transProgressMap[file.first]) | flex | color(Color::White) | bold,
          text(file.second ? " 传输完成" : " 正在传输")
        }) | border
      );
    }

    // 主界面布局
    return vbox({
      text("文件传输列表") | bold | center,
      separator(),
      vbox(file_entries) | frame | flex,
      separator(),
      text("按 ESC 退出") | center
    }) | border | color(Color::White) | bgcolor(Color::RGB(22, 22, 30));
  }) | CatchEvent([](Event event) {
    if(event == Event::Escape) {
      fileTranScreen.Exit();
      return true;
    } else {
      return false;
    }
  });

  fileTranScreen.Loop(renderer);
}