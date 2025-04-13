#ifndef CHANNEL_H
#define CHANNEL_H

#include<functional>
#include"../base/noncopyable.h"
#include"../base/Timestamp.h"

namespace ilib {
namespace net {

class EventLoop;
class Channel : noncopyable
{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(base::Timestamp)>;
    Channel(EventLoop*loop,int fd);
    void handleEvent(base::Timestamp);
    inline void setReadCallback(const ReadEventCallback & cb);
    inline void setWriteCallback(const EventCallback & cb);
    inline void setErrorCallback(const EventCallback & cb);
    inline void setCloseCallback(const EventCallback & cb);

    inline int fd() const;
    inline int events() const;
    inline void set_revents(int revt);
    inline bool isNoneEvent() const;

    void enableReading();
    void enableWriting();
    void disableWriting();
    void disableAll();

    inline int index();
    inline void set_index(int idx);

    inline EventLoop *ownerLoop();

    inline bool isWriting() const;

    void setRevents(int revent) { revents_ = revent; }; // epoll using
private:
    void update();

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop * loop_;
    const int fd_;
    int events_;
    int revents_;
    int index_;

    ReadEventCallback readCallback;
    EventCallback writeCallback;
    EventCallback errorCallback;
    EventCallback closeCallback;
};

void Channel::setReadCallback(const ReadEventCallback &cb) {
    readCallback = cb;
}

void Channel::setWriteCallback(const EventCallback &cb) {
    writeCallback = cb;
}

void Channel::setErrorCallback(const EventCallback &cb) {
    errorCallback = cb;
}

void Channel::setCloseCallback(const EventCallback &cb) {
    closeCallback = cb;
}

int Channel::fd() const{
    return fd_;
}

int Channel::events() const{
    return events_;
}

void Channel::set_revents(int revt) {
    revents_ = revt;
}

bool Channel::isNoneEvent() const {
    return events_ == kNoneEvent;
}

int Channel::index() {
    return index_;
}

void Channel::set_index(int idx) {
    index_ = idx;
}

EventLoop * Channel::ownerLoop() {
    return loop_;
}

bool Channel::isWriting() const {
    return events_ & kWriteEvent;
}

}
}

#endif