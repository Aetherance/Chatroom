#include"Epoller.h"
#include<sys/epoll.h>
#include"../base/logger.h"
#include"Channel.h"

const int _knew = -1;
const int _kadded = 1;
const int _kdel_ed = 2;

using namespace ilib::net;
using namespace ilib::base;

Epoller::Epoller(EventLoop * loop) 
       : ownerLoop_(loop),
         epollfd_(epoll_create1(EPOLL_CLOEXEC)),
         events_(EventSize)
       {}

Epoller::~Epoller() {}

Timestamp Epoller::poll(int timeoutMs,ChannelList &activeChannels) {
    int numEvents = ::epoll_wait(epollfd_,events_.data(),events_.size(),timeoutMs);
    Timestamp now(Timestamp::now());
    if(numEvents > 0) {
        fillActiveChannels(numEvents,activeChannels);
        if(numEvents == events_.size()) {
            events_.resize(2 * events_.size());
        }
    } else if(numEvents == 0) {
        LOG_INFO("epoll: nothing happend!");
    } else {
        LOG_WARN("epoll ret val < 0");
    }
    return now;
}

void Epoller::fillActiveChannels(int numEvents,ChannelList &activeChannels) const {
    for(int i = 0;i<numEvents;i++) {
        Channel * channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->setRevents(events_[i].events);
        activeChannels.push_back(channel);
    } 
}

void Epoller::updateChannel(Channel * channel) {
    assertInLoopThread();
    int index = channel->index();
    if(index == _knew || index == _kdel_ed) {
        int fd = channel->fd();
        if(index == _knew) {
            channels_[fd] = channel;
        }
        channel->set_index(_kadded);
        update(EPOLL_CTL_ADD,channel);
    } else {
        int fd = channel->fd();
        if(channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL,channel);
            channel->set_index(_kdel_ed);
        } else {
            update(EPOLL_CTL_MOD,channel);
        }
    }
}

void Epoller::removeChannel(Channel* channel) {
    assertInLoopThread();
    int fd = channel->fd();
    int index = channel->index();
    channels_.erase(fd);
    if(index == _kadded) {
        update(EPOLL_CTL_DEL,channel);
    }
    channel->set_index(_knew);
}

void Epoller::update(int opt,Channel * channel) {
    ::epoll_event event;
    memset(&event,0,sizeof(event));
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    if(::epoll_ctl(epollfd_,opt,fd,&event) < 0) {
        if(opt == EPOLL_CTL_DEL) {
            LOG_ERROR("epoll : delete error");
        } else {
            LOG_ERROR("opoll : add / mod error");
        }
    }
}