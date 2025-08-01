#include "client.h"
#include <iomanip>
#include <vector>
#include <chrono>
#include "Timestamp.h"
#include <climits>  // 添加头文件用于INT_MAX

using namespace ftxui;

extern std::unordered_map<std::string, std::vector<messageinfo>> messageMap;
extern std::unordered_map<std::string, bool> NewMessageMap;
extern bool isGroupBreak;
extern std::string show_info;

std::string MessageKey;
std::string PeerUserName;
std::string PeerEmail;

std::string getTime(int64_t rawtime);

ScreenInteractive MsgScreen = ScreenInteractive::Fullscreen();
// 状态变量 - 现在存储行级别的滚动偏移
std::unordered_map<std::string, int> MsgScreenScrollOffset;

// 计算可见行数（动态更新）
int getVisibleLines() {
    return Terminal::Size().dimy - 13;
}

std::string input_content;

Component input;
Component send_btn;
Component file_btn;
Component setting_btn;

// ================== 优化的文本换行函数 ==================
// 计算字符串的显示宽度（考虑中文字符）
size_t getDisplayWidth(const std::string& str) {
    size_t width = 0;
    for (size_t i = 0; i < str.length();) {
        unsigned char c = static_cast<unsigned char>(str[i]);
        if (c < 0x80) {
            width++; // ASCII字符宽度为1
            i++;
        } else if ((c & 0xE0) == 0xC0) {
            width += 2; // 中文字符宽度为2
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            width += 2; // 中文字符宽度为2
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            width += 2; // 宽字符宽度为2
            i += 4;
        } else {
            // 非法UTF-8序列，跳过
            i++;
        }
    }
    return width;
}

// 分割文本为多行（考虑中英文宽度）
std::vector<std::string> splitLines(const std::string& input, size_t max_width) {
    std::vector<std::string> lines;
    if (input.empty() || max_width == 0) {
        return lines;
    }

    std::string current_line;
    size_t current_width = 0;

    for (size_t i = 0; i < input.length();) {
        unsigned char c = static_cast<unsigned char>(input[i]);
        size_t char_width = 0;
        size_t char_bytes = 0;

        if (c < 0x80) {
            char_width = 1;
            char_bytes = 1;
        } else if ((c & 0xE0) == 0xC0) {
            char_width = 2;
            char_bytes = 2;
        } else if ((c & 0xF0) == 0xE0) {
            char_width = 2;
            char_bytes = 3;
        } else if ((c & 0xF8) == 0xF0) {
            char_width = 2;
            char_bytes = 4;
        } else {
            char_width = 1;
            char_bytes = 1;
        }

        // 处理换行符
        if (c == '\n') {
            lines.push_back(current_line);
            current_line.clear();
            current_width = 0;
            i += char_bytes;
            continue;
        }

        // 如果添加当前字符会超出宽度限制
        if (current_width + char_width > max_width && !current_line.empty()) {
            lines.push_back(current_line);
            current_line.clear();
            current_width = 0;
        }

        // 添加当前字符
        current_line += input.substr(i, char_bytes);
        current_width += char_width;
        i += char_bytes;
    }

    if (!current_line.empty()) {
        lines.push_back(current_line);
    }

    return lines;
}

// 创建带换行的消息元素（返回行元素列表）
Elements createWrappedLines(const std::string& prefix, const std::string& content, 
                            size_t max_width, bool is_self) {
    // 计算前缀宽度
    size_t prefix_width = getDisplayWidth(prefix);
    
    // 创建完整消息文本
    std::string full_text = prefix + content;
    
    // 分割文本行
    auto lines = splitLines(full_text, max_width);
    
    Elements line_elements;
    for (size_t i = 0; i < lines.size(); i++) {
        // 第一行包含前缀，后续行添加缩进
        if (i == 0) {
            line_elements.push_back(text(lines[i]));
        } else {
            // 创建缩进（使用空格模拟前缀宽度）
            std::string indent(prefix_width, ' ');
            line_elements.push_back(text(indent + lines[i]));
        }
    }
    
    // 设置颜色
    if (is_self) {
        for (auto& element : line_elements) {
            element |= color(Color::GrayLight);
        }
    }
    return line_elements;
}

void Client::MsgController() {
    // 输入组件及配置
    MessageKey = msgClient_.peerEmail();
    PeerUserName = msgClient_.peerUsername();
    PeerEmail = msgClient_.peerEmail();

    NewMessageMap[msgClient_.peerEmail()] = false;

    msgClient_.pullDownloadList(msgClient_.LocalEmail(), msgClient_.peerEmail());

    if (msgClient_.isPeerGroup()) {
        pullGroupMembers();
        msgClient_.pullGroupOwner(false, msgClient_.peerEmail());
        msgClient_.pullGroupOPs(false, msgClient_.peerEmail());
    }

    auto input_option = InputOption();

    input_option.on_enter = [&, this] {
        if (!input_content.empty()) {
            if (parseCommand(input_content)) {
                return;
            }

            if (input_content == "\n") {
                input_content.clear();
                return;
            }

            messageMap[MessageKey].push_back({"You", input_content, ilib::base::Timestamp::now().microSecondsSinceEpoch()});
            msgClient_.sendMsgPeer(input_content);
            input_content.clear();
            
            // 强制滚动到底部（行级别）
            MsgScreenScrollOffset[msgClient_.peerEmail()] = INT_MAX;
        }
    };

    input = Input(&input_content, "输入消息", input_option) | CatchEvent([&](Event event) {
        if(event == Event::CtrlI) {
            return true;
        } else {
            return false;
        }
    });

    // 发送按钮
    send_btn = Button("发送", [&] {
        if (!input_content.empty()) {
            if (parseCommand(input_content)) {
                return;
            }

            messageMap[MessageKey].push_back({"You", input_content, ilib::base::Timestamp::now().microSecondsSinceEpoch()});
            msgClient_.sendMsgPeer(input_content);
            input_content.clear();
            
            // 新消息自动滚动到底部（行级别）
            MsgScreenScrollOffset[msgClient_.peerEmail()] = INT_MAX;
        }
    });

    file_btn = Button("文件...", [&] {
        fileService();
    });

    setting_btn = Button("设置...", [&] {
        if (msgClient_.isPeerGroup()) {
            GroupSettings();
            if (isGroupBreak) {
                MsgScreen.Exit();
            }
        } else {
            UserSettings();
        }
    });

    // 初始位置在底部（行级别）
    MsgScreenScrollOffset[msgClient_.peerEmail()] = INT_MAX;

    auto renderer = makeRenderer();
    // 运行界面
    MsgScreen.Loop(renderer);
}

Element Client::makeSidebar() {
    // 使用局部变量确保不修改原始数据
    const auto members = msgClient_.getGroupMembers(msgClient_.peerEmail());
    static auto lastMembers = members; // 静态变量用于比较变化

    if (msgClient_.isPeerGroup()) {
        // 检查成员列表是否变化
        bool membersChanged = (lastMembers != members);
        lastMembers = members; // 更新缓存

        Elements sidebarText;
        for (const auto& entry : members) { // 使用const避免修改
            if (entry.empty()) continue; // 安全处理空条目

            // 安全提取成员信息和等级
            const char memberLevel = entry.back();
            const std::string memberName = entry.substr(0, entry.size() - 1);

            // 创建元素
            Element text_show;
            if (memberLevel == GROUP_OWNER[0]) {
                text_show = text(memberName + " (群主)") | bold | color(Color::DarkGoldenrod);
            } else if (memberLevel == GROUP_OP[0]) {
                text_show = text(memberName + " 🛡️") | color(Color::CyanLight);
            } else {
                text_show = text(memberName);
            }
            sidebarText.push_back(text_show);
        }

        // 返回群成员面板
        return vbox({
            text("群成员") | bold | center,
            separator(),
            vbox(std::move(sidebarText)) // 移动语义优化
                | flex | vscroll_indicator | frame
        }) | border;
    } else {
        return vbox(); // 非群组时返回空
    }
}

Component Client::makeRenderer() {
    auto layout = Container::Horizontal({input, send_btn, file_btn, setting_btn});

    return Renderer(layout, [&] {
        // 获取当前可见行数
        int visible_lines = getVisibleLines();
        
        // 获取终端宽度用于文本换行
        int terminal_width = Terminal::Size().dimx;
        // 计算最大文本宽度（减去边栏、边框等空间）
        int max_text_width = terminal_width - 25; // 25是预估的边框、边栏和滚动条宽度
        
        // 确保最小宽度
        if (max_text_width < 20) max_text_width = 20;

        // 构建所有行的列表
        Elements all_lines;
        for (const auto& msg : messageMap[MessageKey]) {
            // 创建前缀字符串（时间+发送者）
            std::string prefix = getTime(msg.timestamp) + msg.from + ": ";
            
            // 确定是否是自己发送的消息
            bool is_self = (msg.from == "You" || msg.from == "");
            
            // 获取消息的所有行元素
            Elements msg_lines = createWrappedLines(prefix, msg.text, max_text_width, is_self);
            
            // 添加到总行列表
            all_lines.insert(all_lines.end(), msg_lines.begin(), msg_lines.end());
        }
        
        // 计算总行数
        int total_lines = all_lines.size();
        
        // 处理滚动偏移
        int scroll_offset = MsgScreenScrollOffset[msgClient_.peerEmail()];
        if (scroll_offset == INT_MAX) {
            // 强制滚动到底部
            scroll_offset = total_lines - visible_lines;
        }
        
        // 确保滚动位置在有效范围内
        scroll_offset = std::max(0, std::min(scroll_offset, total_lines - visible_lines));
        
        // 保存调整后的偏移
        MsgScreenScrollOffset[msgClient_.peerEmail()] = scroll_offset;
        
        // 计算可见行范围
        int start_line = scroll_offset;
        int end_line = std::min(total_lines, start_line + visible_lines);
        
        // 提取可见行
        Elements visible_lines_elements;
        if (start_line < total_lines) {
            for (int i = start_line; i < end_line; i++) {
                visible_lines_elements.push_back(all_lines[i]);
            }
        }

        // 基本边栏框架
        auto sidebar = makeSidebar();

        return vbox(
            hbox({
                text(" 💬  " + show_info)
            }) | size(HEIGHT, EQUAL, show_info.empty() ? 0 : 1) | (show_info.empty() ? size(WIDTH, EQUAL, 0) : border),
            // 主聊天区域
            hbox({
                vbox({
                    text(PeerUserName) | bold | center,
                    text(PeerEmail) | center,
                    separator(),
                    vbox(visible_lines_elements) // 只渲染可见行
                        | vscroll_indicator
                        | frame
                        | yflex
                        | flex_shrink
                        | border,
                    separator(),
                    hbox({
                        input->Render() | flex,
                        send_btn->Render(),
                        file_btn->Render(),
                        setting_btn->Render()
                    }) | border
                }) | flex,

                // 右侧边栏
                sidebar
            }) | border | flex
        );
    }) | CatchEvent([&](Event event) -> bool {
        // 定义滚动步长（行数）
        const int ARROW_SCROLL_STEP = 1;   // 键盘箭头滚动一行
        const int WHEEL_SCROLL_STEP = 1;   // 鼠标滚轮滚动一行

        if (event == Event::Escape) {
            MsgScreen.Exit();
            return true;
        }

        // 键盘滚动控制 - 按行滚动
        if (event == Event::ArrowUp) {
            MsgScreenScrollOffset[msgClient_.peerEmail()] -= ARROW_SCROLL_STEP;
            return true;
        }
        if (event == Event::ArrowDown) {
            MsgScreenScrollOffset[msgClient_.peerEmail()] += ARROW_SCROLL_STEP;
            return true;
        }
        if (event == Event::PageUp) {
            MsgScreenScrollOffset[msgClient_.peerEmail()] -= getVisibleLines();
            return true;
        }
        if (event == Event::PageDown) {
            MsgScreenScrollOffset[msgClient_.peerEmail()] += getVisibleLines();
            return true;
        }

        // 鼠标滚轮控制 - 按行滚动
        if (event.is_mouse()) {
            auto& mouse = event.mouse();
            if (mouse.button == Mouse::WheelUp) {
                MsgScreenScrollOffset[msgClient_.peerEmail()] -= WHEEL_SCROLL_STEP;
                return true;
            }
            if (mouse.button == Mouse::WheelDown) {
                MsgScreenScrollOffset[msgClient_.peerEmail()] += WHEEL_SCROLL_STEP;
                return true;
            }
        }
        
        // Home键滚动到顶部
        if (event == Event::Home) {
            MsgScreenScrollOffset[msgClient_.peerEmail()] = 0;
            return true;
        }
        
        // End键滚动到底部
        if (event == Event::End) {
            MsgScreenScrollOffset[msgClient_.peerEmail()] = INT_MAX;
            return true;
        }

        return false;
    }) | color(Color::White) | bgcolor(Color::RGB(22, 22, 30));
}

std::string getTime(int64_t microSecondsSinceEpoch) {
    using namespace std::chrono;

    auto timePoint = time_point<system_clock>(microseconds(microSecondsSinceEpoch));

    time_t time = system_clock::to_time_t(timePoint);

    struct tm localTime;

    if (localtime_r(&time, &localTime) == nullptr) {
        return "[Invalid Time] ";
    }

    std::stringstream ss;
    ss << std::put_time(&localTime, "%H:%M");
    return "[" + ss.str() + "] ";
}