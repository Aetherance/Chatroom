#include "client.h"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/table.hpp>
#include <ftxui/screen/color.hpp>
#include <ctime>
#include <iomanip>
#include <algorithm>

using namespace ftxui;

std::vector<std::string> info_list;

void Client::infos() {
    auto screen = ScreenInteractive::Fullscreen();
    int current_page = 0;
    int items_per_page = 10; // 初始值，会在渲染时更新

    // 计算总页数
    auto calc_total_pages = [&] {
        return info_list.empty() ? 1 : (info_list.size() + items_per_page - 1) / items_per_page;
    };

    // 渲染组件
    auto renderer = Renderer([&] {
        // 根据终端大小计算每页显示的项目数
        int available_height = screen.dimy() - 8; // 预留标题和页脚空间
        items_per_page = std::max(1, available_height);
        
        int total_pages = calc_total_pages();
        if (current_page >= total_pages && total_pages > 0) {
            current_page = total_pages - 1;
        }

        // 获取当前页数据
        int start_index = current_page * items_per_page;
        int end_index = std::min(start_index + items_per_page, static_cast<int>(info_list.size()));
        
        // 创建信息列表元素
        Elements items;
        for (int i = start_index; i < end_index; ++i) {
            // 交替行背景色提高可读性
            auto bg_color = (i % 2 == 0) ? Color::GrayDark : Color::GrayLight;
            items.push_back(hbox({
                text(" • "),
                text(info_list[i]) | flex
            }) | bgcolor(bg_color));
        }

        // 页码信息
        std::string page_info = "页码: " + 
            std::to_string(current_page + 1) + "/" + 
            std::to_string(total_pages) + 
            "  总条目: " + std::to_string(info_list.size());
        
        // 构建界面
        return vbox({
            // 标题
            hbox({
                text("系统信息展示") | bold | center | color(Color::YellowLight)
            }) | border,
            
            // 信息列表
            vbox(items) | flex | frame | border,
            
            // 页脚操作提示
            hbox({
                text(page_info) | flex | align_right,
                text(" ←上一页(p) "),
                text(" 下一页(n)→ "),
                text(" 退出(q) ")
            }) | border | color(Color::BlueLight)
        });
    });

    // 处理键盘事件
    auto component = CatchEvent(renderer, [&](Event event) {
        int total_pages = calc_total_pages();
        
        if (event == Event::Character('q') || event == Event::Escape) {
            screen.Exit();
            return true;
        }
        
        if (event == Event::Character('n') || event == Event::ArrowRight) {
            if (current_page < total_pages - 1) {
                current_page++;
            }
            return true;
        }
        
        if (event == Event::Character('p') || event == Event::ArrowLeft) {
            if (current_page > 0) {
                current_page--;
            }
            return true;
        }
        
        return false;
    });

    screen.Loop(component);
}