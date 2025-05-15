#include"client.h"
#include<iostream>
#include<regex>
#include<termios.h>

using namespace ftxui;

/* 构造函数 : 初始化UI界面 */
Client::Client() : loginScreen_(ScreenInteractive::Fullscreen()),
                   registerScreen_(ScreenInteractive::Fullscreen()),
                   mainScreen_(ScreenInteractive::Fullscreen())
{}

Client::~Client() {

}

/* 运行客户端程序 */
void Client::run() {
  /* 先进行身份验证操作(包含登录和注册) */
  // Verify();
  Msg();
  /* 聊天室聊天功能 */
}

void Client::Verify() {
  userClient_.Connect();
  /* 登录回调 */
  Component login_button = Button("登录",[&]{ LoginController(); mainScreen_.Exit(); });
  /* 注册回调 */
  Component register_button = Button("注册",[&]{ RegisterController(); });
  /* 退出 */
  Component exit_button = Button("退出",[&]{ mainScreen_.Exit(); });

  Component container;

  container = Container::Vertical({
    login_button,
    register_button,
    exit_button
  });

  auto renderer = Renderer(container, [&] {
    return vbox({
      text("▄▖▌   ▗          "),
      text("▌ ▛▌▀▌▜▘▛▘▛▌▛▌▛▛▌"),
      text("▙▖▌▌█▌▐▖▌ ▙▌▙▌▌▌▌"),
      text(""),
      login_button->Render(),
      register_button->Render(),
      exit_button->Render() | bold
    }) | (size(WIDTH, EQUAL, 30) | size(HEIGHT, EQUAL, 20)) | center;
  }) | CatchEvent([&](Event event){
    if(event == Event::Escape) {
      mainScreen_.Exit();
      return true;
    } else {
      return false;
    }
  });

  mainScreen_.Loop(renderer);
}

bool isValidEmail(const std::string& email) {
  const std::regex pattern(
      R"(^(?!.*[.]{2})[a-zA-Z0-9._%+-]+@(?:[a-zA-Z0-9-]+\.)+[a-zA-Z]{2,}$)"
  );
  return std::regex_match(email, pattern);
}

void Client::Msg() {
  printf("\a");
  
  int debug;
  std::cin>>debug;

  if(debug) {
    msgClient_.setEmail("op");
    msgClient_.connect();  
    msgClient_.updatePeer("2085163736@qq.com","the_ink");
    while (true) {
      getchar();
      msgClient_.sendMsgPeer("Hello World!");
    }
  }
  
  std::thread recvThread([this]{ msgClient_.recvMsgLoop(); });
  
  msgClient_.setEmail("2085163736@qq.com");
  msgClient_.connect();
  FriendList();
}