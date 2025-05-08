#include"client.h"

using namespace ftxui;

void Client::DeleteFriend() {
  auto screen = ScreenInteractive::Fullscreen();

  // 使用多个并行数组管理状态
  std::vector<std::string> friends = {"Alice", "Bob", "Charlie", "David"};
  std::vector<bool> deleted(friends.size(), false);
  std::vector<bool> blocked(friends.size(), false);

  // 创建垂直容器
  auto container = Container::Vertical({});

  // 为每个好友创建界面组件
  for (size_t i = 0; i < friends.size(); ++i) {
    // 删除按钮回调
    auto delete_btn = Button("删除", [&, i] {
      deleted[i] = true;
      screen.PostEvent(Event::Custom);
    });

    // 拉黑按钮回调
    auto block_btn = Button("拉黑", [&, i] {
      blocked[i] = !blocked[i]; // 切换拉黑状态
      screen.PostEvent(Event::Custom);
    });

    // 创建带边框的好友条目
    auto entry = Container::Horizontal({
        Renderer([&, i] {
          return text(" " + friends[i] + 
                     (blocked[i] ? " (已拉黑)" : "")) | flex;
        }),
        delete_btn,
        block_btn
    }) | border;

    container->Add(entry);
  }

  // 主渲染器
  auto renderer = Renderer(container, [&] {
    Elements entries;

    // 添加标题
    entries.push_back(hbox({text(" 好友管理 ") | bold | center}) | borderDouble);

    // 渲染可见条目
    for (size_t i = 0; i < friends.size(); ++i) {
      if (!deleted[i]) {
        auto element = container->ChildAt(i)->Render();
        if (blocked[i]) {
          element |= color(Color::RedLight);
        }
        entries.push_back(element);
        entries.push_back(separator());
      }
    }

    // 空状态提示
    if (entries.size() == 1) {
      entries.push_back(text("暂无好友") | center | dim | border);
    }

    return vbox(entries) | border | flex | frame;
  }) | CatchEvent([&](Event event) -> bool {
    if(event == Event::Escape) {
      screen.Exit();
      return true;
    } else {
      return false;
    }
  });

  screen.Loop(renderer);
}