// #include"client.h"
// #include<iomanip>
// #include<vector>
// #include <chrono>

// using namespace ftxui;

// struct ChatMessage {
//   std::string time;
//   std::string user;
//   std::string content;
// };

// class MessageBox : public ComponentBase {
//   public:
//       MessageBox(std::vector<ChatMessage>& messages, 
//                 std::mutex& mutex,
//                 int& message_count)
//           : messages_ref(messages), 
//             mutex_ref(mutex),
//             message_count_ref(message_count) 
//       {
//           // 滚动状态管理
//           box = Renderer([this] {
//               std::lock_guard<std::mutex> lock(mutex_ref);
//               Elements elements;
              
//               // 生成消息元素
//               for (auto& msg : messages_ref) {
//                   elements.push_back(hbox({
//                       text("[" + msg.time + "] ") | color(Color::GrayDark),
//                       text("<" + msg.user + "> ") | color(Color::Green),
//                       text(msg.content)
//                   }));
//               }
              
//               // 自动滚动处理
//               if (prev_count != messages_ref.size()) {
//                   scroll_to_bottom = true;
//                   prev_count = messages_ref.size();
//               }
              
//               // 构建可滚动区域
//               auto box = vbox(elements);
//               if (scroll_to_bottom) {
//                   box = box | vscroll_indicator | yframe | yflex;
//                   scroll_to_bottom = false;
//               } else {
//                   box = box | vscroll_indicator | yflex;
//               }
              
//               return box;
//           });
          
//           Add(box);
//       }

//   private:
//       std::vector<ChatMessage>& messages_ref;
//       std::mutex& mutex_ref;
//       int& message_count_ref;
//       Component box;
//       size_t prev_count = 0;
//       bool scroll_to_bottom = true;
//   };

// void Client::MsgController() {
//   std::vector<ChatMessage> messages;
//   std::mutex messages_mutex;
//   std::string input_buffer;
//   std::atomic<bool> running{true};
//   int scroll_position = 0;

//   auto screen = ScreenInteractive::Fullscreen();

//   // 消息显示组件（带自动滚动）
//   class MessageBox : public ComponentBase {
//   public:
//       MessageBox(std::vector<ChatMessage>& msgs, 
//                std::mutex& mtx,
//                int& scroll_pos)
//           : messages_ref(msgs), 
//             mutex_ref(mtx),
//             scroll_pos_ref(scroll_pos) 
//       {
//           Component inner = Renderer([this] {
//               std::lock_guard<std::mutex> lock(mutex_ref);
              
//               // 生成消息元素
//               Elements elements;
//               for (auto& msg : messages_ref) {
//                   elements.push_back(hbox({
//                       text("[" + msg.time + "] ") | color(Color::GrayDark),
//                       text("<" + msg.user + "> ") | color(Color::Green),
//                       text(msg.content)
//                   }));
//               }

//               // 自动滚动处理
//               if (messages_ref.size() != prev_count) {
//                   scroll_pos_ref = elements.size(); // 新消息时滚动到底部
//                   prev_count = messages_ref.size();
//               }

//               // 带滚动条的容器
//               return vbox(elements) | vscroll_indicator | frame | 
//                      focus | vscroll_offset(scroll_pos_ref);
//           });

//           Add(inner);
//       }

//   private:
//       std::vector<ChatMessage>& messages_ref;
//       std::mutex& mutex_ref;
//       int& scroll_pos_ref;
//       size_t prev_count = 0;
//   };

//   // 输入组件
//   auto input = Input(&input_buffer, "输入消息...");
  
//   // 发送按钮
//   auto send_btn = Button("发送", [&] {
//       if (!input_buffer.empty()) {
//           std::lock_guard<std::mutex> lock(messages_mutex);
//           auto now = std::chrono::system_clock::now();
//           auto t = std::chrono::system_clock::to_time_t(now);
//           messages.push_back({
//               std::ctime(&t),
//               "用户",
//               input_buffer
//           });
//           input_buffer.clear();
//       }
//   });

//   // 消息框组件（传递滚动位置引用）
//   int scroll_pos = 0;
//   auto message_box = std::make_shared<MessageBox>(messages, messages_mutex, scroll_pos);

//   // 布局设置
//   auto layout = Container::Vertical({
//       message_box,
//       Container::Horizontal({input, send_btn})
//   });

//   // 渲染器
//   auto renderer = Renderer(layout, [&] {
//       return vbox({
//           message_box->Render() | flex,
//           separator(),
//           hbox({
//               input->Render() | flex,
//               send_btn->Render()
//           })
//       }) | border;
//   });

//   // 事件处理
//   auto main_component = CatchEvent(renderer, [&](Event event) {
//       if (event == Event::Escape) {
//           running = false;
//           screen.Exit();
//           return true;
//       }
//       return false;
//   });

//   // 消息接收线程
//   std::thread receiver([&] {
//       int counter = 0;
//       while (running) {
//           std::this_thread::sleep_for(std::chrono::seconds(3));
//           {
//               std::lock_guard<std::mutex> lock(messages_mutex);
//               messages.push_back({
//                   "系统时间",
//                   "机器人",
//                   "自动消息 #" + std::to_string(++counter)
//               });
//           }
//           screen.PostEvent(Event::Custom);
//       }
//   });

//   screen.Loop(main_component);
//   running = false;
//   receiver.join();
// }