#include"client.h"

using namespace ftxui;

// 数据结构定义[1,3](@ref)
struct GroupApplication {
  std::string username;
  std::string group_name;
  bool approved = false;
  bool rejected = false;
};

std::vector<GroupApplication> applications = {
  {"User_A", "开发者交流群", false},
  {"User_B", "技术讨论组", false},
  {"User_C", "项目协作群", false},
  {"User_D", "产品反馈群", false},
  {"User_E", "测试环境群", false}
};

void Client::GroupVerify() {
  auto screen = ScreenInteractive::Fullscreen();
  
  // 动态申请列表组件[6,7](@ref)
  int selected_application = 0;
  auto application_list = Container::Vertical({});
  
  // 按钮回调处理[4](@ref)
  auto process_approval = [&](int index, bool approve) {
      applications[index].approved = approve;
      applications[index].rejected = !approve;
      if (selected_application >= (int)applications.size()-1)
          selected_application = std::max(0, (int)applications.size()-2);
  };

  // 构建动态列表[7](@ref)
  for (size_t i = 0; i < applications.size(); ++i) {
      auto approve_btn = Button(" 批准 ", [=, &screen] { 
          process_approval(i, true);
          screen.ExitLoopClosure()();
      });
      
      auto reject_btn = Button(" 拒绝 ", [=, &screen] {
          process_approval(i, false);
          screen.ExitLoopClosure()();
      });

      application_list->Add(
          Container::Horizontal({
              approve_btn,
              reject_btn
          })
      );
  }

  // 界面渲染器[6](@ref)
  auto renderer = Renderer(application_list, [&] {
      Elements entries;
      for (size_t i = 0; i < applications.size(); ++i) {
          auto& app = applications[i];
          if (app.approved || app.rejected) continue;

          auto entry = 
              hbox({
                  text(" ➤ 用户: ") | bold | color(Color::Green),
                  text(app.username + "  ") | flex,
                  text("申请加入: ") | bold | color(Color::Yellow),
                  text(app.group_name) | flex,
                  separator(),
                  application_list->ChildAt(i)->Render()
              }) | borderLight;
              
          if (i == selected_application) 
              entry |= inverted;
          
          entries.push_back(entry);
      }
      
      // 布局框架[6](@ref)
      return vbox({
          text(" 群聊管理系统 ") | bold | hcenter | color(Color::Blue),
          separator(),
          window(text("待处理申请 (" + std::to_string(applications.size()) + ")") | hcenter,
              vbox(entries) | focusPosition(0, selected_application) | frame | vscroll_indicator | flex
          ),
          text("方向键选择 | Enter确认 | ESC退出") | dim | hcenter
      }) | borderDouble | color(Color::White);
  });

  // 键盘导航处理[7](@ref)
  auto component = CatchEvent(renderer, [&](Event event) {
      if (event == Event::ArrowUp && selected_application > 0) {
          selected_application--;
          return true;
      }
      if (event == Event::ArrowDown && selected_application < (int)applications.size()-1) {
          selected_application++;
          return true;
      }
      return false;
  });

  screen.Loop(component);
}