#include"client.h"

using namespace ftxui;

void Client::setOp() {
  // std::string email;
  // std::string status_message;
  // bool operation_success = false;
  // bool last_action_was_set = true; // 记录最后一次操作类型

  // auto screen = ScreenInteractive::TerminalOutput();
  
  // auto & members = msgClient_.getGroupMembers(msgClient_.peerEmail());

  // // 邮箱输入框组件
  // Component email_input = Input(&email, "输入成员邮箱");

  // // 操作按钮组件
  // Component set_button = Button("设为管理员", [&] {
  //   last_action_was_set = true;
  //   if (email.empty()) {
  //     status_message = "错误：邮箱不能为空";
  //     operation_success = false;
  //     return;
  //   }

  //   bool isExist = false;

  //   for(auto m : members) {
  //     if(m.find(email) != m.npos && email.find('@') != m.npos && email.find('.') != m.npos) {
  //       isExist = true;
  //     }
  //   }

  //   if( !isExist) {
  //     status_message = "错误： 成员不存在";
  //     operation_success = false;
  //     return;
  //   } 
    
  //   msgClient_.setOP(email,msgClient_.peerEmail());
    
  //   status_message = "已设置为管理员: " + email;
  //   email.clear();
  // });

  // Component remove_button = Button("取消管理员", [&] {
  //   last_action_was_set = false;
  //   if (email.empty()) {
  //     status_message = "错误：邮箱不能为空";
  //     operation_success = false;
  //     return;
  //   }

  //   bool isExist = false;

  //   for(auto m : members) {
  //     if(m.find(email) != m.npos && email.find('@') != m.npos && email.find('.') != m.npos) {
  //       isExist = true;
  //     }
  //   }

  //   if( !isExist) {
  //     status_message = "错误： 成员不存在";
  //     operation_success = false;
  //     return;
  //   }  
    
  //   msgClient_.deOP(email,msgClient_.peerEmail());

  //   status_message = "已取消管理员权限: " + email;
  //   email.clear();
  // });

  // // 退出按钮组件
  // Component quit_button = Button("退出", [&] {
  //   screen.Exit();
  // });

  // // 主布局组件
  // auto layout = Container::Vertical({
  //   email_input,
  //   Container::Horizontal({
  //     set_button,
  //     remove_button,
  //     quit_button
  //   })
  // });

  // // 渲染器
  // auto renderer = Renderer(layout, [&] {
  //   // 标题
  //   auto title = text("管理员权限管理") | bold | center | color(Color::Cyan);
    
  //   // 状态消息（使用emptyElement替代nothing）
  //   Element status_text = emptyElement();
  //   if (!status_message.empty()) {
  //     auto color = operation_success ? Color::Green : Color::Red;
  //     status_text = text(status_message) | center;
  //   }
    
  //   // 操作模式提示
  //   auto mode_hint = text(last_action_was_set ? "操作模式: 设置管理员" : "操作模式: 取消管理员") | center;
    
  //   // 按钮组
  //   auto buttons = hbox({
  //     set_button->Render(),
  //     text("  "),
  //     remove_button->Render(),
  //     text("  "),
  //     quit_button->Render()
  //   }) | center;
    
  //   // 主界面
  //   return vbox({
  //     vbox({
  //       title,
  //       separator(),
  //       hbox(text("邮箱: "), email_input->Render()) | borderEmpty,
  //       separator(),
  //       mode_hint,
  //       separator(),
  //       buttons,
  //       separator(),
  //       status_text
  //     }) | border | size(WIDTH, LESS_THAN, 70),
  //   }) | flex | center;
  // });

  // screen.Loop(renderer);
}