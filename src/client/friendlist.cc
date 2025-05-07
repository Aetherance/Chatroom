#include"client.h"

using namespace ftxui;

void Client::FriendList() {
  std::vector<std::string> friends = {"op"};
  std::string new_friend;
  bool in_chat = false;

  auto screen = ScreenInteractive::Fullscreen();

  // 输入组件
  Component input = Input(&new_friend, "添加新好友...");

  // 添加好友按钮
  Component add_button = Button("搜索", [&] {
    if (!new_friend.empty()) {
      friends.push_back(new_friend);
      new_friend.clear();
    }
  });

  Component verify_button = Button("验证信息", [&] {
    
  });

  // 好友列表容器
  Component friends_container = Container::Vertical({});
  
  // 主界面组件
  auto main_component = Container::Vertical({
    Container::Horizontal({input, add_button , verify_button}),
    friends_container
  });

  // 渲染逻辑
  auto main_renderer = Renderer(main_component, [&] {
    if (in_chat) {
      // 聊天界面
      MsgController();
      in_chat = false;
    }
    
    // 好友列表界面
    friends_container->DetachAllChildren();
    for (const auto& name : friends) {
      auto btn = Button(name, [&, name] { 
        msgClient_.updatePeer("email",name);
        in_chat = true;
      });
      friends_container->Add(btn);
    }
    
    return vbox({
      hbox({
        input->Render() | flex,
        add_button->Render(),
        verify_button->Render()
      }) | border,
      friends_container->Render() | vscroll_indicator | frame | flex,
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