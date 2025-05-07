#include"client.h"

using namespace ftxui;

void Client::FriendList() {
  std::vector<std::string> friends = {"op"};
  std::string new_friend;
  bool in_chat = false;
  std::string chatting_with;
  
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

  // 好友列表容器
  Component friends_container = Container::Vertical({});
  
  // 主界面组件
  auto main_component = Container::Vertical({
    Container::Horizontal({input, add_button}),
    friends_container
  });

  // 渲染逻辑
  auto main_renderer = Renderer(main_component, [&] {
    if (in_chat) {
      // 聊天界面
      auto back_button = Button("返回", [&] { in_chat = false; });
      return vbox({
        text("正在和 " + chatting_with + " 聊天"),
        separator(),
        back_button->Render(),
      }) | border | center;
    }
    
    // 好友列表界面
    friends_container->DetachAllChildren();
    for (const auto& name : friends) {
      auto btn = Button(name, [&, name] { 
        chatting_with = name;
        in_chat = true;
      });
      friends_container->Add(btn);
    }
    
    return vbox({
      hbox({
        input->Render() | flex,
        add_button->Render(),
      }) | border,
      friends_container->Render() | vscroll_indicator | frame | flex,
    }) | border;
  });

  screen.Loop(main_renderer);
}