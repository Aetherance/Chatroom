#include"EventLoopThreadPool.h"
using namespace ilib::net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop * baseloop) 
    : baseLoop_(baseloop),
      started_(false),
      numThreads_(0),
      next_(0) {}

EventLoopThreadPool::~EventLoopThreadPool() {}

void EventLoopThreadPool::start() {
    baseLoop_->assertInLoopThread();
    started_ = true;
    for(int i = 0;i< numThreads_;i++) {
        std::shared_ptr<EventLoopThread> thread(new EventLoopThread);
        threads_.push_back(thread);
        loops_.push_back(thread->startLoop());
    }
}

EventLoop * EventLoopThreadPool::getNextLoop() {
    baseLoop_->assertInLoopThread();
    EventLoop * loop;
    if(!loops_.empty()) {
        loop = loops_[next_];
        next_ = (next_ + 1) % loops_.size();
    }
    return loop;
}
