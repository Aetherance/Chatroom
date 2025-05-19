#include"client.h"

using namespace ftxui;

void Client::DeleteGroup() {
  auto screen = ScreenInteractive::Fullscreen();

  // 群聊列表状态
  auto groups = std::make_shared<std::vector<std::string>>();
  groups->assign({"技术交流群", "游戏组队群", "学习互助群", "同城交友群"});

  // 列表组件容器
  auto list_component = Container::Vertical({});

  // 更新列表的函数
  auto update_list = [&] {
    list_component->DetachAllChildren();
    
    for (const auto& group : *groups) {
      auto btn = Button("退出", [&] {
        // 从列表中移除对应群聊
        auto it = std::find(groups->begin(), groups->end(), group);
        if (it != groups->end()) {
          groups->erase(it);
          // 触发界面更新
          screen.PostEvent(Event::Custom);
        }
      });
      
      // 创建水平布局：群聊名称 + 退出按钮
      auto row = Container::Horizontal({
        Renderer([group] { 
          return text("  " + group) | flex | bgcolor(Color::RGB(22,22,30)); 
        }),
        btn
      });
      
      list_component->Add(row);
    }
  };

  // 初始创建列表
  update_list();

  // 主界面布局
  auto main_component = Renderer(list_component, [&] {
    Element content = list_component->Render() | frame | vscroll_indicator;
    
    // 空状态提示
    if (groups->empty()) {
      content = vbox({
        text("您还没有加入任何群聊"),
        text("按ESC键返回")
      }) | center;
    }

    return window(
      text("退出群聊") | bold | center,
      vbox({
        content | flex,
        filler(),
        text("使用方向键导航，Enter键操作") | center | dim
      })
    ) | size(WIDTH, GREATER_THAN, 50) | 
      size(HEIGHT, GREATER_THAN, 20) | 
      borderDouble | color(Color::White) | bgcolor(Color::RGB(22,22,30));
  });

  // 添加退出快捷键
  auto component = CatchEvent(main_component, [&](Event event) {
    if (event == Event::Escape) {
      screen.Exit();
      return true;
    }
    return false;
  });

  screen.Loop(component);
}