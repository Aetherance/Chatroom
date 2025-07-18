#include "client.h"

using namespace ftxui;

extern ScreenInteractive FriendListScreen;
auto user_setting_screen = ScreenInteractive::Fullscreen();

std::unordered_map<std::string, bool> isBlocked;

void Client::UserSettings() {
    // 使用局部引用确保始终访问最新状态
    bool & blocked = isBlocked[msgClient_.peerEmail()];
    
    auto button0 = Button("取消拉黑", [=,&blocked] {
        msgClient_.unBlock(msgClient_.LocalEmail(), msgClient_.peerEmail());
        blocked = false;  // 直接修改引用
    });

    auto button1 = Button("拉黑用户", [=,&blocked] {
        msgClient_.blockFriend(msgClient_.LocalEmail(), msgClient_.peerEmail());
        blocked = true;  // 直接修改引用
    });

    auto button2 = Button("返回", [&] { 
        user_setting_screen.Exit();
    });
    
    auto container = Container::Vertical({
        button2,
        Maybe(button0, &blocked),      // 当blocked=true时显示
        Maybe(button1, [&]{ 
            return !blocked; // 当blocked=false时显示
        })
    });

    // 渲染器定义布局
    auto renderer = Renderer(container, [&] {
        return vbox({
            filler(),
            vbox({
                button2->Render() 
                    | size(WIDTH, EQUAL, 20) 
                    | color(Color::Black),
                    
                blocked ? 
                    button0->Render() | size(WIDTH, EQUAL, 20) | color(Color::RedLight) :
                    button1->Render() | size(WIDTH, EQUAL, 20) | color(Color::Red),
            }) | border | center,
            filler()
        }) | hcenter | border;
    }) | CatchEvent([](Event event) {
        if (event == Event::Escape) {
            user_setting_screen.Exit();
            return true;
        }
        return false;
    });

    user_setting_screen.Loop(renderer);
}