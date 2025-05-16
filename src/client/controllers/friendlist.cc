#include"client.h"

using namespace ftxui;

std::vector<std::pair<std::string,std::string>> friends = { {"op","op"}};

void Client::FriendList() {
  /* 第一个是邮箱 第二个是用户名 */
  std::string new_friend;
  bool in_chat = false;

  auto screen = ScreenInteractive::Fullscreen();

  // 输入组件
  std::string input_show = "添加新好友...";
  Component input = Input(&new_friend, input_show);

  // 添加好友按钮
  Component add_button = Button("添加", [&] {
    if (!new_friend.empty()) {
      // friends.push_back(onew_friend);
      new_friend.clear();
    }
  });

  Component verify_button = Button("验证信息", [&] {
    VerifyFriend();
  });

  Component delete_button = Button("删除好友", [&] {
    DeleteFriend();
  });

  Component group_button = Button("群聊", [&] {
    GroupList();
  });

  // 好友列表容器
  Component friends_container = Container::Vertical({});

  // 主界面组件
  auto main_component = Container::Vertical({
    Container::Horizontal({input, add_button , verify_button , delete_button , group_button}),
    friends_container,
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
      auto btn = Button(name.second, [&, name] { 
        msgClient_.updatePeer(name.first,name.second);
        in_chat = true;
      });
      friends_container->Add(btn);
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
        text("好友列表"),
        friends_container->Render() | vscroll_indicator | frame,
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