#include"client.h"

using namespace ftxui;
 
bool isValidEmail(const std::string &);

void Client::LoginController() {
  std::string email_in;
  std::string passwd_in;
  std::string fake_passwd;

  Component container;
  Component login_button;
  Component passwd_input;
  Component email_input;

  std::string info;

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
    container->SetActiveChild(login_button);
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

  login_button = Button("登录", [&] {
    if (email_in.empty() || passwd_in.empty()) {
      info =  "密码或邮箱不能为空!";
    } else if( ! isValidEmail(email_in) && email_in != "op"){
      info = "邮箱格式有误!";
    } else if(email_in.size() > 20 || passwd_in.size() > 20) {
      info = "邮箱或密码长度过长!";
    } else if( ! userClient_.isConnected()) {
      info = "登录服务器未连接!";
    } else {
      info = "验证中";
      bool isSuccess = LoginSubmit(email_in,passwd_in,info);
      email_in.clear();
      passwd_in.clear();
      fake_passwd.clear();
      if(isSuccess) {
        userClient_.setHasLogin(true);
        loginScreen_.Exit();
        return;
      }
    }
  });
  
  container = Container::Vertical({
    email_input,
    passwd_input,
    login_button
  });

  auto renderer = Renderer(container, [&] {
    return vbox({
      text("Chatroom") | bold,
      text("登录"),
      email_input->Render() | border,
      passwd_input->Render() | border,
      login_button->Render() | size(WIDTH, EQUAL, 40) | center,
      text(info)
    }) | (size(WIDTH, EQUAL, 30) | size(HEIGHT, EQUAL, 20)) | center;
  }) | CatchEvent([&](Event event) {
    if(event == Event::CtrlC) {
      return true;
    } else {
      return false;
    }
  });


  loginScreen_.Loop(renderer);
}

bool Client::LoginSubmit(const std::string & email,const std::string & passwd,std::string & info) {
  int recv = userClient_.RequestLogin(email,passwd);
  if(recv == USER_OK) {
    localUserEmail_ = email;
    loginScreen_.Exit();
    mainScreen_.Exit();
    return true;
  } else if(recv == USER_HAVE_LOGIN_ED) {
    info = "用户已登陆!";
    return false;
  }
  else {
    return false;
    info = "帐号或密码错误!";
  }
}