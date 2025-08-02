#include"client.h"

using namespace ftxui;

struct Member {
  std::string email;
  std::string name;
  bool selected = false;
};

extern std::string show_info;

std::vector<Member> mems = {};

void Client::kickMember() {
  auto screen = ScreenInteractive::Fullscreen();

  std::string group_name = msgClient_.peerEmail();

  std::vector<std::string> me = msgClient_.getGroupMemberEmail(group_name);
  
  std::vector<std::string> m = msgClient_.getGroupMembers(group_name);

  mems.clear();

  for(int i = 0;i<m.size();i++) {
    if( !msgClient_.isGroupOwner(me[i],group_name) && msgClient_.LocalEmail() != me[i]) {
      m[i].resize(m[i].size()-1);
      mems.push_back({me[i],m[i],false});
    }
  }

  auto member_list = Container::Vertical({});

  for (auto& member : mems) {
      member_list->Add(
          Checkbox(member.name, &member.selected)
      );
  }

  auto btn_confirm = Button("踢！", [&,this]{
    for(auto & m : mems) {
      if(m.selected) {
        msgClient_.rmGroupMember(m.email,msgClient_.peerEmail());
      }
    }

    msgClient_.pullGroupMembers(false,group_name);

    showInfo("你移除了一个群的群成员!");
    screen.Exit();
  });
  auto btn_cancel = Button("返回", screen.ExitLoopClosure());

  // 布局（保持原有结构）
  auto layout = Container::Vertical({
      member_list,
      Container::Horizontal({btn_confirm, btn_cancel})
  });

  // 渲染器（优化显示效果）
  auto renderer = Renderer(layout, [&] {
      return vbox({
          // 标题栏
          hbox({text(" 请选择要踢出的群成员 ") | bold | bgcolor(Color::Red) | color(Color::White)}) | center,
          separator(),

          // 主内容区域
              vbox({
                  separator(),
                  window(text("选择成员") | bold,
                      vbox({member_list->Render() | yframe | flex})) | flex
              }) | flex,

          separator(),

          // 按钮区域
          hbox({
              btn_confirm->Render() | borderLight | color(Color::RedLight),
              text(" "),
              btn_cancel->Render() | borderLight
          }) | center
      }) | border | color(Color::White) | bgcolor(Color::RGB(22, 22, 30));
  });

  screen.Loop(renderer);
}