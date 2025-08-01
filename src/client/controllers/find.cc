#include "client.h"

using namespace ftxui;

extern std::vector<Friend>users;

std::string info = "";

auto findScreen = ScreenInteractive::Fullscreen();

extern std::vector<Friend> friends;

void Client::FindFriend() {
  msgClient_.pullAllUsers();

  auto main_container = Container::Vertical({});
  
  std::vector<Friend> display_users = users;

  // 为每个用户创建界面元素
  for (size_t i = 0; i < display_users.size(); ++i) {
    std::string user_id = display_users[i].email;
    
    if(user_id == msgClient_.LocalEmail()) {
      continue;
    }

    auto add_btn = Button("加好友", [&, user_id] {
      msgClient_.addFriend(msgClient_.LocalEmail(), user_id);

      info = "好友申请已发送!";

      std::thread([]{ sleep(3); info = ""; findScreen.PostEvent(Event::Custom); }).detach();

      findScreen.PostEvent(Event::Custom);
    });

    auto add_ed_btn = Button("已添加", [&, user_id] {
      info = "已经添加过该好友!";

      std::thread([]{ sleep(3); info = ""; findScreen.PostEvent(Event::Custom); }).detach();

      findScreen.PostEvent(Event::Custom);
    });

    bool isExist = false;

    for(auto f : friends) {
      if(f.email == user_id) {
        isExist = true;
      }
    }

    auto line = Container::Horizontal({
        Renderer([i] { return text(" " + users[i].username + " (" + users[i].email  + ")") | flex; }),
        ( !isExist ? add_btn : add_ed_btn),
    }) | border;

    main_container->Add(line);
  }

  auto renderer = Renderer(main_container, [&] {
    Elements contents;

    // 添加标题
    contents.push_back(hbox({
        text(" 发现 ") | bold | center
    }) | border);

    for (size_t i = 0; i < main_container->ChildCount(); ++i) {
      auto element = main_container->ChildAt(i)->Render();
      contents.push_back(element);
    }

    contents.push_back( text(info) | (info.empty() ? size(HEIGHT,EQUAL,0) : size(HEIGHT,EQUAL,1)));

    // 处理空列表的情况
    if (contents.size() == 1) {
      contents.push_back(text("暂无用户") | center | dim);
    }

    return vbox(contents) | border | flex | frame;
  }) | border | color(Color::White) | bgcolor(Color::RGB(22, 22, 30))
  | CatchEvent([&](Event event) -> bool {
    if(event == Event::Escape) {
      findScreen.Exit();
      return true;
    } else {
      return false;
    }
  });

  findScreen.Loop(renderer);
}