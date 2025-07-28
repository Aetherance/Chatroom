#include"client.h"

using namespace ftxui;

bool isFirstLogin = true;

std::vector<Friend> friends = {};

std::string show_info;

extern std::unordered_map<std::string,std::vector<messageinfo>> messageMap;

extern std::unordered_map<std::string,bool> NewMessageMap;

ScreenInteractive FriendListScreen = ScreenInteractive::Fullscreen();

std::string getCurrentTimePeriod();

void Client::FriendList() {
  msgClient_.pullFriendList();

  msgClient_.pullAllUsers();

  if(isFirstLogin) {
    showInfo("👋 " + getCurrentTimePeriod() + "好!");
    isFirstLogin = false;
  } else {
    show_info = "";
  }

  /* 第一个是邮箱 第二个是用户名 */
  bool in_chat = false;
  
  std::string new_friend;

  // 输入组件
  std::string input_show = "输入邮箱以添加新好友...";
  Component input = Input(&new_friend, input_show)  | CatchEvent([&](Event event){
    if(event == Event::Return) {
      return true;
    } else {
      return false;
    }
  });
  
  // 添加好友按钮
  Component add_button = Button("添加", [&] {
    if(new_friend == msgClient_.LocalEmail()) {
      showInfo("不可以添加自己为好友!");
      new_friend.clear();
      return ;
    }
    if (!new_friend.empty()) {
      msgClient_.addFriend(msgClient_.LocalEmail(),new_friend);
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

  Component find_button = Button("发现",[&] {
    FindFriend();
  });

  Component cancel_button = Button("设置...", [&] {
    Settings();
    if(isExit || isLogout) {
      FriendListScreen.Exit();
    }
  });

  Component infos_button = Button("消息",[&] {
    infos();
  });

  Component reload = Button("↻ 刷新", [&] {
    msgClient_.pullFriendList();
  });

  // 好友列表容器
  Component friends_container = Container::Vertical({});

  // 主界面组件
  auto main_component = Container::Vertical({
    Container::Horizontal({input, add_button , verify_button , delete_button , group_button , find_button , infos_button ,reload}),
    friends_container,
    cancel_button
  });

  // 渲染逻辑
  auto main_renderer = Renderer(main_component, [&] {
    if (in_chat) {
      // 聊天界面
      msgClient_.setIsPeerGroup(false);
      MsgController();
      in_chat = false;
    }
    
    // 好友列表界面
    friends_container->DetachAllChildren();
    for (const auto& name : friends) {
      auto btn = Button((name.isOnline ? " ● " : " ○ ") + name.username + (NewMessageMap[name.email] ? " 💬" : ""), [&, name] { 
        msgClient_.updatePeer(name.email,name.username);
        in_chat = true;
      });
      friends_container->Add(btn);
    }

    return vbox({
      text("Chatroom") | bold | center,
      hbox({
        text(" 💬  " + show_info)
      }) | size(HEIGHT,EQUAL, show_info.empty() ? 0 : 1) | (show_info.empty() ? size(WIDTH,EQUAL,0) : border),
      hbox({
        input->Render() | flex,
        add_button->Render(),
        verify_button->Render(),
        delete_button->Render(),
        group_button->Render(),
        find_button->Render(),
        infos_button->Render(),
        reload->Render()
      }) | border,
      vbox({
        text("好友列表"),
        friends_container->Render() | vscroll_indicator | frame | flex,
        cancel_button->Render()
      }) | flex
    }) | border;
  }) | CatchEvent([&](Event event) -> bool { 
    if(event == Event::Escape && in_chat) {
      in_chat = false;
      return true;
    } else if(event == Event::CtrlC) {
      isExit = true;
      return false;
    } else {
      return false;
    }
  }) | bgcolor(Color::RGB(22, 22, 30));

  FriendListScreen.Loop(main_renderer);
}

std::string getCurrentTimePeriod() {
    time_t now = time(nullptr);
    tm *local = localtime(&now);
    int hour = local->tm_hour;
    
    if (hour >= 5 && hour < 12) 
        return "上午";
    else if (hour >= 12 && hour < 14)
        return "中午";
    else if (hour >= 14 && hour < 19)
        return "下午";
    else
        return "晚上";
}