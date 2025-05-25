#include"client.h"

using namespace ftxui;

extern std::vector<Friend> friends;

void Client::DeleteFriend() {
  auto screen = ScreenInteractive::Fullscreen();

  // 创建垂直容器
  auto container = Container::Vertical({});

  // 为每个好友创建界面组件
  for (size_t i = 0; i < friends.size(); ++i) {
    // 删除按钮回调
    auto delete_btn = Button("删除", [&, i] {
      msgClient_.deleteFriend(msgClient_.LocalEmail(),friends[i].email);
      friends.erase(friends.begin() + i);
      screen.PostEvent(Event::Custom);
    });

    // 创建带边框的好友条目
    auto entry = Container::Horizontal({
        Renderer([&, i] {
          return text(" " + friends[i].username) | flex;
        }),
        delete_btn
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
      auto element = container->ChildAt(i)->Render();
      entries.push_back(element);
      entries.push_back(separator());
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
  }) | color(Color::White) | bgcolor(Color::RGB(22, 22, 30));

  screen.Loop(renderer);
}