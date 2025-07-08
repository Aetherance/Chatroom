#include "client.h"

using namespace ftxui;

auto screen = ScreenInteractive::Fullscreen();

std::string status = {};

std::vector<std::string> downloadable_files;

void Client::fileService() {
  // 状态变量
  std::string upload_path;

  msgClient_.pullDownloadList(msgClient_.LocalEmail(),msgClient_.peerEmail());

  // 上传组件
  auto input_upload = Input(&upload_path, "输入文件路径");
  auto upload_button = Button(" 上传文件 ", [&]{
    if(upload_path.empty()) {
      return ;
    }

    if(!std::filesystem::exists(upload_path)) {
      status = "文件不存在!";
      return ;
    }

    std::string filename = std::filesystem::path(upload_path).filename();

    std::thread([&,upload_path,filename]{ 
      ftpClient_.transList.insert({filename,false});

      ftpClient_.transProgressMap[filename] = 0.0f;

      sendFileTo(msgClient_.peerEmail() + "/" + msgClient_.LocalEmail(),filename,upload_path);
    }).detach();
    
    upload_path.clear();
    
    fileTrans();
  });
  
  auto upload_section = Container::Horizontal({
    input_upload,
    upload_button
  });

  // 下载按钮组件
  std::vector<Component> download_buttons;
  for (size_t i = 0; i < downloadable_files.size(); ++i) {
    download_buttons.push_back(
      Button(downloadable_files[i], [&,i] {
        std::thread([&]{
          ftpClient_.transList.insert({downloadable_files[i],false});

          ftpClient_.transProgressMap[downloadable_files[i]] = 0.0f;

          std::filesystem::path savePath = "./download";
          std::filesystem::create_directory(savePath);
          ftpClient_.downloadFile(savePath,msgClient_.LocalEmail() + "/" + msgClient_.peerEmail(),downloadable_files[i]);
        }).detach();

        fileTrans();
      })
    );
  }
  auto download_section = Container::Vertical(download_buttons);

  // 传输列表按钮
  auto transfer_list_button = Button(" 传输列表 ", [&]{ fileTrans(); });

  // 组合所有组件
  auto main_container = Container::Vertical({
    upload_section,
    download_section,
    transfer_list_button
  });

  // 渲染界面 - 占满全屏
  auto renderer = Renderer(main_container, [&] {
    return vbox({
      // 标题区域 (高度固定)
      hbox({
        filler(),
        text("文件传输系统") | bold | center,
        filler(),
      }) | size(HEIGHT, EQUAL, 3),
      
      separator(),

      text(" " + status) | size(HEIGHT,EQUAL, status.empty() ? 0 : 1) | ( status.empty() ?  bold : border),
      
      // 上传区域 (高度固定)
      vbox({
        text("上传文件:") | bold,
        hbox({
          input_upload->Render() | flex | border,
          upload_button->Render()
        })
      }) | border | size(HEIGHT, EQUAL, 6) | bold,
      
      separator(),
      
      // 下载区域 (占据剩余空间)
      vbox({
        text("可下载文件:") | bold,
        download_section->Render() | flex | frame
      }) | border | flex,
      
      separator(),
      
      // 底部区域 (高度固定)
      hbox({
        filler(),
        transfer_list_button->Render(),
        filler()
      }) | size(HEIGHT, EQUAL, 3)
    }) | flex | border | bold;
  }) | color(Color::White) | bgcolor(Color::RGB(22, 22, 30));

  screen.Loop(renderer);
}