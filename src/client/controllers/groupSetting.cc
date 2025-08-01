#include"client.h"

using namespace ftxui;

extern ScreenInteractive FriendListScreen;

extern ScreenInteractive MsgScreen;
 
extern std::unordered_map<std::string,std::vector<messageinfo>> messageMap;

auto group_setting_screen = ScreenInteractive::Fullscreen();

bool isGroupBreak = false;

void Client::GroupSettings() {
  msgClient_.pullGroupOwner(false,msgClient_.peerEmail());

  msgClient_.pullGroupOPs(false,msgClient_.peerEmail());

  bool isLocalGroup = msgClient_.isGroupOwner(msgClient_.LocalEmail(),msgClient_.peerEmail());

  bool isOpGroup = msgClient_.isGroupOp(msgClient_.LocalEmail(),msgClient_.peerEmail());

  auto button1 = Button("设置管理员", [=]{
    setOp();
  });

  auto button2 = Button("踢出成员", [&]{
    kickMember();
  });

  auto button3 = Button("解散群聊", [&]{ 
    group_setting_screen.Exit();
    messageMap[msgClient_.peerEmail()].clear();
    msgClient_.breakGroup(msgClient_.LocalEmail(),msgClient_.peerEmail());
    isGroupBreak = true;
    msgClient_.pullGroupList();
  });
  
  auto button4 = Button("返回", [&]{ 
    group_setting_screen.Exit();
  });

  auto container = Container::Vertical({
    button4,
    button1,
    button2,
    button3
  });

  // 渲染器定义布局
  auto renderer = Renderer(container, [&] {
    return vbox({
      filler(),
      
      vbox({
        button4->Render() | size(WIDTH, EQUAL, 20),
        button1->Render() | (isLocalGroup ? size(WIDTH, EQUAL, 20) : size(HEIGHT, EQUAL, 0) ),
        button2->Render() | ((isLocalGroup | isOpGroup) ? size(WIDTH, EQUAL, 20) : size(HEIGHT, EQUAL, 0) ),
        button3->Render() | (isLocalGroup ? size(WIDTH, EQUAL, 20) : size(HEIGHT, EQUAL, 0) )
      }) | border | center,
      
      filler()
    }) | hcenter | border;
  }) | CatchEvent([](Event event){
    if(event == Event::Escape) {
      group_setting_screen.Exit();
      return true;
    } else {
      return false;
    }
  });

  group_setting_screen.Loop(renderer);
}