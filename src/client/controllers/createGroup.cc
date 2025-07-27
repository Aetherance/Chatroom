#include "client.h"
#include <ftxui/component/component.hpp>

using namespace ftxui;

extern std::vector<Friend> friends;

struct Member {
  std::string email;
  std::string name;
  bool selected = false;
};
  
  // 使用结构体代替两个并行vector
std::vector<Member> members = {};

void Client::CreateGroup() {
  auto screen = ScreenInteractive::Fullscreen();

  std::string group_name;

  members.clear();

  for(auto & Friend : friends) {
    members.push_back({Friend.email,Friend.username,false});
  }

  // 组件定义
  auto input_group = Input(&group_name, "输入群聊名称") | CatchEvent([&](Event event) {
    if(event == Event::Return) {
      return true;
    } else {
      return false;
    }
  });

  auto member_list = Container::Vertical({});
  for (auto& member : members) {
      member_list->Add(
          Checkbox(member.name, &member.selected)
      );
  }

  auto btn_confirm = Button("✓ 确认创建", [&,this]{
    std::vector<std::string>groupMembers;
    if(group_name.empty()) {
      return ;
    }
    for(auto & member : members) {
      if(member.selected) {
        groupMembers.emplace_back(member.email);
      }
    }
    msgClient_.createGroup(msgClient_.LocalEmail(),group_name,groupMembers);
    screen.Exit();
  });
  auto btn_cancel = Button("✗ 返回", screen.ExitLoopClosure());

  // 布局（保持原有结构）
  auto layout = Container::Vertical({
      input_group,
      member_list,
      Container::Horizontal({btn_confirm, btn_cancel})
  });

  // 渲染器（优化显示效果）
  auto renderer = Renderer(layout, [&] {
      return vbox({
          // 标题栏
          hbox({text(" 创建新群聊 ") | bold | bgcolor(Color::BlueLight) | color(Color::White)}) | center,
          separator(),

          // 主内容区域
          window(text("群聊信息") | bold,
              vbox({
                  hbox({text("名称: "), input_group->Render() | flex}),
                  separator(),
                  window(text("选择成员") | bold,
                      vbox({member_list->Render() | yframe | flex})) | flex
              })
          ) | flex,

          separator(),

          // 按钮区域
          hbox({
              btn_confirm->Render() | borderLight | color(Color::GreenLight),
              text(" "),
              btn_cancel->Render() | borderLight | color(Color::RedLight)
          }) | center
      }) | border | color(Color::White) | bgcolor(Color::RGB(22, 22, 30));
  });

  screen.Loop(renderer);
}