#include"EventLoop.h"
#include"TimerQueue.h"
#include"TimerId.h"
#include<algorithm>
#include"../base/Timestamp.h"
#include"sys/timerfd.h"
#include<assert.h>
#include<unistd.h>
#include<memory.h>

using namespace ilib::net;

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
    std::vector<TimerQueue::Entry> ret_expired;
    Entry sentry = std::make_pair(now,reinterpret_cast<Timer*>(UINTPTR_MAX));
    auto it = timers_.lower_bound(sentry);
    std::copy(timers_.begin(),it,back_inserter(ret_expired));
    timers_.erase(timers_.begin(),it);
    return ret_expired;
}

TimerId TimerQueue::addTimer(const Timer::TimerCallback &cb,Timestamp when,double interval) {
    Timer* timer = new Timer(cb,when,interval);
    loop_->runInloop([this,timer]{
        addTimerInLoop(timer);
    });
    return TimerId(timer);
}

void TimerQueue::addTimerInLoop(Timer * timer) {
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);

    if(earliestChanged) {
        resetTimerfd(timerfd_,timer->expiration());
    }
}

struct timespec howMuchTimeFromNow(ilib::base::Timestamp when){
    int64_t microseconds = when.microSecondsSinceEpoch() - ilib::base::Timestamp::now().microSecondsSinceEpoch();
    if(microseconds < 100){
        microseconds = 100;
    }

    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / 1000000);
    ts.tv_nsec = static_cast<long>((microseconds % 1000000) * 1000);
    return ts;
}

void TimerQueue::resetTimerfd(int timerfd,Timestamp expiration) {
    itimerspec newVal;
    itimerspec oldVal;
    bzero(&newVal,sizeof(newVal));
    bzero(&oldVal,sizeof(oldVal));
    newVal.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd,0,&newVal,&oldVal);
    if(ret) {
        perror("timerfd_settime");
    }
}

void TimerQueue::reset(const std::vector<Entry>& expired,Timestamp now) {
    Timestamp nextExpire(0);

    for(auto& entry : expired) {
        ActiveTimer timer(entry.second,entry.second->sequence());
        if(entry.second->repeat() && cancelingTimers_.find(timer) == cancelingTimers_.end()) {
            entry.second->restart(now);
            insert(entry.second);
        } else {
            delete entry.second;
        }
    }

    if(timers_.empty()) {
        nextExpire = timers_.begin()->second->expiration();
    }

    if(nextExpire.valid()) {
        resetTimerfd(timerfd_,nextExpire);
    }
}

bool TimerQueue::insert(Timer * timer) {
    loop_->assertInLoopThread();

    bool earliestChanged = false;
    Timestamp when = timer->expiration();

    auto it = timers_.begin();
    if(it == timers_.end() || when < it->first) {
        earliestChanged = true;
    }

    // 错误检测 并 插入
    {
        std::pair<TimerList::iterator,bool> result = timers_.insert(Entry(when,timer));
        assert(result.second);
        (void)result; // 显式忽略未使用变量
    }

    {
        std::pair<ActiveTimerSet::iterator,bool> result = activeTimerset_.insert(ActiveTimer(timer,timer->sequence()));
        assert(result.second);
        (void)result;
    }

    assert(timers_.size() == activeTimerset_.size());
    return earliestChanged;
}

TimerQueue::TimerQueue(EventLoop * loop) 
        : loop_(loop), 
          timerfd_(createTimerfd()),
          timerfdChannel_(loop,timerfd_),
          timers_() {}

int TimerQueue::createTimerfd() {
    return timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK | TFD_CLOEXEC);
}

void TimerQueue::cancel(TimerId timerid) {
    loop_->runInloop([this,timerid]{cancelInLoop(timerid);});
}

void TimerQueue::cancelInLoop(TimerId timerid) {
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimerset_.size());
    ActiveTimer timer(timerid.timer_,timerid.seq_);
    ActiveTimerSet::iterator it = activeTimerset_.find(timer);
    if(it!=activeTimerset_.end()) {
        size_t n = timers_.erase(Entry(it->first->expiration(),it->first));
        assert(n == 1);
        (void)n;
        delete it->first;
        activeTimerset_.erase(it);
    } else if(callingExpiredTimers_) {
        auto it = cancelingTimers_.find(timer);
        if(it != cancelingTimers_.end()) {
            size_t n = timers_.erase(Entry(it->first->expiration(),it->first));
            assert(n == 1);
            (void)n;
            delete it->first;
            cancelingTimers_.erase(it);
        }
    }
}

void readTimerfd(int timerfd,ilib::base::Timestamp now) {
    uint64_t howmany;
    ssize_t n = ::read(timerfd,&howmany,sizeof(howmany));
    if(n != sizeof(howmany)) {
        perror("read");
    }
}

void TimerQueue::handleRead() {
    loop_->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd_,now);

    std::vector<Entry>expired = getExpired(now);

    callingExpiredTimers_ = true;
    cancelingTimers_.clear();

    for(auto&entry : expired) {
        entry.second->run();
    }

    callingExpiredTimers_ = false;

    reset(expired,now);
}

TimerQueue::~TimerQueue() {
    ::close(timerfd_);
    for(auto & timer : timers_) {
        delete timer.second;
    }
}