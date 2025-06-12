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
          text(file) | flex | size(WIDTH, EQUAL, 16),
          gauge(ftpClient_.transProgressMap[file]) | flex | color(Color::White)
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
  });

  fileTranScreen.Loop(renderer);
}