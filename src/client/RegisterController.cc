#include"Client.h"

using namespace ftxui;
 
bool isValidEmail(const std::string &);

void Client::RegisterController() {
  std::string username_in;
  std::string email_in;
  std::string passwd_in;
  std::string fake_passwd;
  std::string code_in;

  Component container;
  Component register_button;
  Component passwd_input;
  Component email_input;
  Component username_input;
  Component code_input;
  Component send_code_button;

  ScreenInteractive register_screen = ScreenInteractive::Fullscreen();

  std::string info;

  username_input = Input(&username_in, "昵称") | CatchEvent([&](Event event) {
    if(event == Event::Return) {
      container->SetActiveChild(email_input);
      return true;
    }
      return false;
    });

  email_input = Input(&email_in, "邮箱") | CatchEvent([&](Event event) {
  if(event == Event::Return) {
    container->SetActiveChild(passwd_input);
    return true;
  }
    return false;
  });

  passwd_input = Input(&fake_passwd, "密码") | CatchEvent([&](Event event) -> bool {
  if(event.is_character()){
    passwd_in += event.character();
    fake_passwd += '*';
    return true;
  } else if(event == Event::Return || event == Event::ArrowDown) {
    container->SetActiveChild(send_code_button);
    return true;
  } else if(event == Event::Backspace) {
    if(!passwd_in.empty()) {
      passwd_in.pop_back();
    }
    
    if(!fake_passwd.empty()) {
      fake_passwd.pop_back();
    }
    return true;
  }
  else {
    return false;
  }
  });

  code_input = Input(&code_in, "验证码") | CatchEvent([&](Event event) {
    if(event == Event::Return) {
      container->SetActiveChild(passwd_input);
      return true;
    } else if(code_in.size() > 6) {
      info = "验证码长度过长!";
      return true;
    }
      return false;
    });

  send_code_button = Button("发送验证码",[&]{
    if (email_in.empty() || passwd_in.empty()) {
      info =  "密码或邮箱不能为空!";
    } else if( ! isValidEmail(email_in)){
      info = "邮箱格式有误!";
    } else if(email_in.size() > 20 || passwd_in.size() > 20) {
      info = "邮箱或密码长度过长!";
    } else {
      int recv = userClient_.SendRegister1(email_in);
      if(recv == USER_OK) {
        info = "验证码已发送!";
      } else if(recv == EMAIL_ALREADY_REGISTERED) {
        info = "邮箱已存在!";
      };
    }
  });

  register_button = Button("完成注册", [&] {
    if( ! userClient_.isConnected()) {
      info = "注册服务器未连接!";
    } else {
      info = "验证中";
      int recv = userClient_.SendRegister2(email_in,username_in,passwd_in);
      if(recv == USER_OK) {
        register_screen.Exit();
      }
      info = "验证码错误!";
    }
  });
  
  container = Container::Vertical({
    username_input,
    email_input,
    passwd_input,
    send_code_button,
    code_input,
    register_button
  });

  auto renderer = Renderer(container, [&] {
    return vbox({
      text("Chatroom") | bold,
      text("注册"),
      username_input->Render() | border,
      email_input->Render() | border,
      passwd_input->Render() | border,
      send_code_button->Render(),
      code_input->Render() | border,
      register_button->Render() | size(WIDTH, EQUAL, 40) | center,
      text(info)
    }) | (size(WIDTH, EQUAL, 30) | size(HEIGHT, EQUAL, 30)) | center;
  });


  register_screen.Loop(renderer);
}

// bool Client::LoginSubmit(const std::string & email,const std::string & passwd,const std::string & info) {
//   int recv = -1;
//   if(userClient_.RequestLogin(email,passwd) == USER_OK) {
//     return true;
//   } else {
//     return false;
//   }
// }