#include"client.h"

using namespace ftxui;

std::vector<Group> groups = {};

extern std::string show_info;

extern std::unordered_map<std::string,std::vector<messageinfo>> messageMap;

extern std::unordered_map<std::string,bool> NewMessageMap;

ScreenInteractive GroupListScreen = ScreenInteractive::Fullscreen();

void Client::GroupList() {
  pullGroupMembers();

  bool in_chat = false;  

  std::string new_group;

  // 输入组件
  std::string input_show = "输入群名添加新群聊...";
  Component input = Input(&new_group, input_show) | CatchEvent([&](Event event) {
    if(event == Event::Return) {
      return true;
    } else {
      return false;
    }
  });
  
  // 添加好友按钮
  Component add_button = Button("添加", [&] {
    if (!new_group.empty()) {
      msgClient_.addGroup(msgClient_.LocalEmail(),new_group);
      new_group.clear();
    }
  });

  Component verify_button = Button("群聊管理", [&] {
    GroupVerify();
  });

  Component delete_button = Button("退出群聊", [&] {
    DeleteGroup();
  });

  Component create_button = Button("创建群聊", [&] {
    CreateGroup();
  });

  Component group_button = Button("好友", [&] {
    GroupListScreen.Exit();
  });

  Component info_button = Button("信息", [&] {
    infos();
  });

  Component reload = Button("↻ 刷新", [&] {
    msgClient_.pullGroupList();
  });

  // 好友列表容器
  Component friends_container = Container::Vertical({});

  // 主界面组件
  auto main_component = Container::Vertical({
    Container::Horizontal({input, add_button , create_button , verify_button , delete_button , group_button , info_button , reload}),
    friends_container,
  });

  // 渲染逻辑
  auto main_renderer = Renderer(main_component, [&] {
    if (in_chat) {
      msgClient_.setIsPeerGroup(true);
      MsgController();
      in_chat = false;
    }
    
    // 好友列表界面
    friends_container->DetachAllChildren();
    for (const auto& name : groups) {
      auto btn = Button(name.groupname + + (NewMessageMap[name.groupname] ? " 💬" : ""), [&, name] { 
        msgClient_.updatePeer(name.groupname,name.groupname);
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
        create_button->Render(),
        verify_button->Render(),
        delete_button->Render(),
        group_button->Render(),
        info_button->Render(),
        reload->Render()
      }) | border,
      vbox({
        text("群聊列表"),
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
  }) | bgcolor(Color::RGB(22, 22, 30));

  GroupListScreen.Loop(main_renderer);
}