#include"client.h"

using namespace ftxui;

void Client::kickMember() {
  std::string email;
  std::string status_message;
  bool operation_success = false;

  auto & members = msgClient_.getGroupMembers(msgClient_.peerEmail());

  // 邮箱输入框组件
  Component email_input = Input(&email, "输入成员邮箱");

  // 踢出按钮组件
  Component kick_button = Button("踢出成员", [&] {
    if (email.empty()) {
      status_message = "错误：邮箱不能为空";
      operation_success = false;
      return;
    }

    if(email == msgClient_.LocalEmail()) {
      status_message = "不要踢自己!";
      email.clear();     
      return ;
    }

    bool isExist = false;

    for(auto m : members) {
      if(m.find(email) != m.npos && email.find('@') != m.npos && email.find('.') != m.npos) {
        isExist = true;
      }
    }

    if( !isExist) {
      status_message = "错误： 成员不存在";
      operation_success = false;
      return;
    } 
    
    msgClient_.rmGroupMember(email,msgClient_.peerEmail());

    status_message = "成功踢出: " + email;
    email.clear();
  });

  // 主布局组件
  auto layout = Container::Vertical({
    email_input,
    kick_button,
  });

  // 渲染器
  auto renderer = Renderer(layout, [&] {
    Element status_text;
    if (operation_success) {
      status_text = text(status_message) | color(Color::Green);
    } else {
      status_text = text(status_message) | color(Color::Red);
    }
    
    return vbox({
      hbox(text("邮箱: "), email_input->Render()),
      separator(),
      hbox({
        kick_button->Render(),
      }),
      separator(),
      status_text
    }) | border | size(WIDTH, GREATER_THAN, 60) | center;
  });

  auto screen = ScreenInteractive::TerminalOutput();
  screen.Loop(renderer);
}