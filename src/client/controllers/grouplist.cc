#include"client.h"

using namespace ftxui;

void Client::GroupList() {
  std::vector<std::string> groups = {"op","op2"};
  std::string new_group;
  bool in_chat = false;

  auto screen = ScreenInteractive::Fullscreen();

  // 输入组件
  Component input = Input(&new_group, "加入新群聊...");

  // 添加好友按钮
  Component add_button = Button("搜索", [&] {
    if (!new_group.empty()) {
      groups.push_back(new_group);
      new_group.clear();
    }
  });

  Component verify_button = Button("管理群聊", [&] {

  });

  Component delete_button = Button("退出群聊", [&] {
    
  });

  Component group_button = Button("好友", [&] {
    screen.Exit();
  });

  // 好友列表容器
  Component groups_container = Container::Vertical({});

  // 主界面组件
  auto main_component = Container::Vertical({
    Container::Horizontal({input, add_button , verify_button , delete_button , group_button}),
    groups_container,
  });

  // 渲染逻辑
  auto main_renderer = Renderer(main_component, [&] {
    if (in_chat) {
      // 聊天界面
      MsgController();
      in_chat = false;
    }
    
    // 好友列表界面
    groups_container->DetachAllChildren();
    for (const auto& name : groups) {
      auto btn = Button(name, [&, name] { 
        msgClient_.updatePeer("email",name);
        in_chat = true;
      });
      groups_container->Add(btn);
    }

    return vbox({
      text("Chatroom") | bold | center,
      hbox({
        input->Render() | flex,
        add_button->Render(),
        verify_button->Render(),
        delete_button->Render(),
        group_button->Render()
      }) | border,
      vbox({
        text("群聊列表"),
        groups_container->Render() | vscroll_indicator | frame,
      }) | flex
    }) | border;
  }) | CatchEvent([&](Event event) -> bool { 
    if(event == Event::Escape && in_chat) {
      in_chat = false;
      return true;
    } else {
      return false;
    }
  });

  screen.Loop(main_renderer);
}