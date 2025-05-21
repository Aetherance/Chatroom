#include"client.h"

using namespace ftxui;

std::vector<GroupApplication> applications = {};

extern std::string show_info3;

void Client::GroupVerify() {
  auto screen = ScreenInteractive::Fullscreen();
  
  int selected_application = 0;
  auto application_list = Container::Vertical({});
  
  show_info3.clear();
  
  for (size_t i = 0; i < applications.size(); ++i) {
    auto approve_btn = Button(" 通过 ", [=, &screen] { 
      msgClient_.verifyGroup(applications[i].user,applications[i].group);
      applications.erase(applications.begin() + i);
    });
    
    auto reject_btn = Button(" 忽略 ", [=, &screen] {
        
      applications.erase(applications.begin() + i);
    });

    application_list->Add(
      Container::Horizontal({
        approve_btn,
        reject_btn
      })
    );
  }

  auto renderer = Renderer(application_list, [&] {
      Elements entries;
      for (size_t i = 0; i < applications.size(); ++i) {
        auto& app = applications[i];

        auto entry = 
          hbox({
            text(" ➤ 用户: ") | bold | color(Color::Green),
            text(app.user + "  ") | flex,
            text("申请加入: ") | bold | color(Color::Yellow),
            text(app.group) | flex,
            separator(),
            application_list->ChildAt(i)->Render()
          }) | borderLight;
          
        if (i == selected_application) 
          entry |= inverted;
        
        entries.push_back(entry);
      }
      
      return vbox({
          text(" 群聊管理系统 ") | bold | hcenter | color(Color::Blue),
          separator(),
          window(text("待处理申请 (" + std::to_string(applications.size()) + ")") | hcenter,
              vbox(entries) | focusPosition(0, selected_application) | frame | vscroll_indicator | flex
          ),
          text("方向键选择 | Enter确认 | ESC退出") | dim | hcenter
      }) | borderDouble | color(Color::White);
  });

  auto component = CatchEvent(renderer, [&](Event event) {
    if (event == Event::ArrowUp && selected_application > 0) {
      selected_application--;
      return true;
    } else if (event == Event::ArrowDown && selected_application < (int)applications.size()-1) {
      selected_application++;
      return true;
    } else if(event == Event::Escape) {
      screen.Exit();
    }

    return false;
  });

  screen.Loop(component);
}