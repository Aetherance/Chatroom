#ifndef EVENTLOOPTHREAD_H
#define EVENTLOOPTHREAD_H

#include"EventLoop.h"
#include<thread>
#include<mutex>
#include<condition_variable>

namespace ilib {
namespace net {
class EventLoopThread 
{
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop * startLoop();
private:
    void threadFunc();
    std::thread thread_;
    EventLoop * loop_;
    std::mutex mutex_;
    std::condition_variable cond_;
    bool exiting_;
};

inline EventLoopThread::EventLoopThread() :
    loop_(nullptr), exiting_(false) {}

inline EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if(loop_ != nullptr) {
        loop_->quit();
    }
    thread_.join();
}

inline EventLoop * EventLoopThread::startLoop() {
    thread_ = std::thread([this]{ threadFunc(); });

    {
        std::unique_lock lock(mutex_);
        cond_.wait(lock,[this](){ return loop_ != nullptr; });
    }

    return loop_;
}

inline void EventLoopThread::threadFunc() {
    EventLoop loop;

    {
        std::unique_lock lock(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }

    loop.loop();
}

}
}

#endif