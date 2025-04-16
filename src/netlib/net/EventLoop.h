#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include<thread>
#include<memory>
#include"../base/noncopyable.h"
#include<memory>
#include<vector>
#include"TimerQueue.h"
#include"TimerId.h"
#include<mutex>
#include"sigpipe.h"

namespace ilib {
namespace net {

class Epoller;
class Channel;
class EventLoop : noncopyable
{
public:
    EventLoop();
    ~EventLoop();
    
    void loop(int timeout = -1);
    void quit();
    
    void assertInLoopThread();
    bool isInLoopThread() const;
    static EventLoop * getThreadLoop();
    void updateChannel(Channel * channel);
    
    TimerId runAt(const Timestamp &time,const Timer::TimerCallback &cb);
    TimerId runAfter(double delay,const Timer::TimerCallback &cb);
    TimerId runEvery(double interval,const Timer::TimerCallback &cb);

    using Functor = std::function<void()>;
    void runInloop(const Functor &cb);
    void queueInLoop(const Functor& cb);

    void removeChannel(Channel * channel);

private:
    void abortNotInLoopThread();
    void handleRead();
    void doPendingFunctors();

    using ChannelList = std::vector<Channel*>;
    
    bool quit_;
    bool looping_;
    bool callingPendingFunctors_;

    const std::thread::id threadid_;
    
    // Poller
    std::unique_ptr<Epoller>poller_;

    // wakeup
    int wakeupFd_;
    std::unique_ptr<Channel>wakeupChannel_;

    // Channel
    ChannelList activeChannels_;
    
    std::unique_ptr<TimerQueue> timerqueue_;
    std::mutex mtx;
    std::vector<Functor>pendingFunctors_;

    void wakeup();
    int makeWakeUpFd();
};

}
}

#endif