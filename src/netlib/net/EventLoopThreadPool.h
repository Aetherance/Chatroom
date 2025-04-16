#ifndef EVENTLOOPTHREADPOOL_H
#define EVENTLOOPTHREADPOOL_H

#include"../base/noncopyable.h"
#include"EventLoopThread.h"

namespace ilib{
namespace net{

class EventLoopThreadPool : noncopyable
{
public:
    EventLoopThreadPool(EventLoop * baseloop);
    ~EventLoopThreadPool();

    void setThreadNum(int numThreads);
    void start();
    EventLoop * getNextLoop();

private:
    EventLoop * baseLoop_;
    bool started_;
    int numThreads_;
    int next_;
    
    std::vector<std::shared_ptr<EventLoopThread>>threads_;
    std::vector<EventLoop*>loops_;
};

inline void EventLoopThreadPool::setThreadNum(int numThreads) {
    numThreads_ = numThreads;
}

}
}

#endif