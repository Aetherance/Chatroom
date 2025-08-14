#include"client.h"

using namespace ftxui;

#define HISTORY_MESSAGE_FILE ".data"

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
    isExit = true;
    setting_screen.Exit();
    FriendListScreen.Exit();
  });

  auto button3 = Button("返回...", [&]{ 
    setting_screen.Exit();
  });

  auto button4 = Button("登出...",[&]{ 
    setting_screen.Exit();
    isLogout = true;
    userClient_.removeToken();
  });

  Component saveHistory = Button("备份历史聊天记录",[&] {
    showInfo("备份聊天记录...");
    setting_screen.Exit();
    ftpClient_.uploadFile("./history/" + localUserEmail_ + HISTORY_MESSAGE_FILE,"historyMessages",msgClient_.LocalEmail() + HISTORY_MESSAGE_FILE);
  });

  Component resetHistory = Button("恢复历史聊天记录",[&] {
    showInfo("历史聊天记录已恢复!");
    setting_screen.Exit();
    ftpClient_.downloadFile("./history/","historyMessages",msgClient_.LocalEmail() + HISTORY_MESSAGE_FILE);
    readMessage();
  });

  auto container = Container::Vertical({
    button1,
    button2,
    button3,
    button4,
    saveHistory,
    resetHistory
  });

  // 渲染器定义布局
  auto renderer = Renderer(container, [&] {
    return vbox({
      filler(),
      
      vbox({
        button1->Render() | size(WIDTH, EQUAL, 20),
        button2->Render() | size(WIDTH, EQUAL, 20),
        button3->Render() | size(WIDTH, EQUAL, 20),
        button4->Render() | size(WIDTH, EQUAL, 20),
        saveHistory->Render() | size(WIDTH, EQUAL, 20),
        resetHistory->Render() | size(WIDTH, EQUAL, 20)
      }) | border | center,
      
      filler()
    }) | hcenter | border;
  }) | CatchEvent([](Event event){
    if(event == Event::Escape) {
      setting_screen.Exit();
      return true;
    } else {
      return false;
    }
  });

  setting_screen.Loop(renderer);
}