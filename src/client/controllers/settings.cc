#include"client.h"

using namespace ftxui;

extern ScreenInteractive FriendListScreen;
auto setting_screen = ScreenInteractive::Fullscreen();

void Client::Settings() {
  
  auto button1 = Button("退出...", [=]{ 
    isExit = true;
    setting_screen.Exit();
    FriendListScreen.Exit();
  });

  auto button2 = Button("注销...", [&]{
    msgClient_.CancelAccount(msgClient_.LocalEmail());
  });

  auto button3 = Button("返回...", [&]{ 
    setting_screen.Exit();
  });
  

  auto container = Container::Vertical({
    button1,
    button2,
    button3
  });

  // 渲染器定义布局
  auto renderer = Renderer(container, [&] {
    return vbox({
      filler(),
      
      vbox({
        button1->Render() | size(WIDTH, EQUAL, 20),
        button2->Render() | size(WIDTH, EQUAL, 20),
        button3->Render() | size(WIDTH, EQUAL, 20)
      }) | border | center,
      
      filler()
    }) | hcenter | border;
  });

  setting_screen.Loop(renderer);
}