#include"Channel.h"
#include<poll.h>
#include"EventLoop.h"
#include"../base/logger.h"

using namespace ilib::net;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop *loop,int fd) :
         loop_(loop),fd_(fd),events_(kNoneEvent),
         revents_(kNoneEvent),index_(-1) {}

void Channel::update() {
    loop_->updateChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime) {
    if(revents_ & POLLNVAL) {
        LOG_WARN("Chanenl::handle_event() POLLNVAL");
    }

    if((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
        if(closeCallback) {
            closeCallback();
        }
    }

    if(revents_ & (POLLERR | POLLNVAL)) {
        if(errorCallback) {
            errorCallback();
        }
    }

    if(revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
        if(readCallback) {
            readCallback(receiveTime);
        }
    }

    if(revents_ & POLLOUT) {
        if(writeCallback) {
            writeCallback();
        }
    }
}

void Channel::enableReading() {
    events_ |= kReadEvent;
    update();
}

void Channel::enableWriting() {
    events_ |= kWriteEvent;
    update();
}

void Channel::disableWriting() {
    events_ &= ~kWriteEvent;
    update();
}

void Channel::disableAll() {
    events_ = kNoneEvent;
    update();
}