#include"client.h"

using namespace ftxui;

std::vector<std::string> friendRequests = {};
extern std::vector<Friend> friends;

void Client::VerifyFriend() {
  auto screen = ScreenInteractive::Fullscreen();

  // 主容器
  auto main_container = Container::Vertical({});

  // 为每个请求创建界面元素
  for (size_t i = 0; i < friendRequests.size(); ++i) {
    auto req = friendRequests[i];
    
    // 接受按钮回调
    auto accept_btn = Button("接受", [&,i,req] {
      msgClient_.verifyFriend(msgClient_.LocalEmail(),req);
      friendRequests.erase(friendRequests.begin() + i);      
      screen.PostEvent(Event::Custom); // 触发界面更新
    });

    // 拒绝按钮回调
    auto reject_btn = Button("拒绝", [i,req, &screen,this] {
      msgClient_.reject(req);
      friendRequests.erase(friendRequests.begin() + i);
      screen.PostEvent(Event::Custom);
    });

    // 将按钮和文本组合成一行
    auto line = Container::Horizontal({
        Renderer([req] { return text(" " + req + " 的好友申请 ") | flex; }),
        accept_btn,
        reject_btn,
    }) | border;

    main_container->Add(line);
  }

  // 界面渲染器
  auto renderer = Renderer(main_container, [&] {
    Elements contents;

    // 添加标题
    contents.push_back(hbox({
        text(" 好友申请列表 ") | bold | center,
    }) | border);

    // 添加请求条目
    for (size_t i = 0; i < friendRequests.size(); ++i) {
      auto req = friendRequests[i];
      
      if(main_container->ChildCount() > i) {
        auto element = main_container->ChildAt(i)->Render();
        contents.push_back(element);
      }
    }

    // 处理空列表的情况
    if (contents.size() == 1) {
      contents.push_back(text("暂无新好友申请") | center | dim);
    }

    return vbox(contents) | border | flex | frame;
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