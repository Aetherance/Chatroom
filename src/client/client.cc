#include"client.h"
#include<iostream>
#include<regex>

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
  Verify();

  /* 聊天室聊天功能 */
  
}

void Client::Verify() {
  userClient_.Connect();

  Component login_button = Button("登录",[&]{ LoginController(); });
  Component register_button = Button("注册",[&]{ RegisterController(); });
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