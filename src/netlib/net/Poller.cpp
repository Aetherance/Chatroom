#include"Poller.h"
#include<poll.h>
#include"../base/logger.h"
#include"../base/Timestamp.h"
#include<assert.h>
#include"Channel.h"

using namespace ilib::base;
using namespace ilib::net;

Poller::Poller(EventLoop * loop) : ownerLoop_(loop) {}

Poller::~Poller() {}

Timestamp Poller::poll(int timeoutMs,ChannelList &activeChannels) {
    int numEvents = ::poll(pollfds_.data(),pollfds_.size(),timeoutMs);
    Timestamp time(Timestamp::now());
    if(numEvents > 0) {
        fillActiveChannels(numEvents,activeChannels);
    }
    return time;
}

void Poller::fillActiveChannels(int numEvents,ChannelList& activeChannels) const {
    for(int i = 0;i<pollfds_.size() && numEvents > 0;i++) {
        if(pollfds_[i].revents > 0) {
            numEvents --;
            assert(channels_.find(pollfds_[i].fd) != channels_.end());
            Channel* channel = (*channels_.find(pollfds_[i].fd)).second;
            assert(channel->fd() == pollfds_[i].fd);
            channel->set_revents(pollfds_[i].revents);
            activeChannels.push_back(channel);
        }
    }
}

void Poller::updateChannel(Channel * channel) {
    assertInLoopThread();
    if(channel->index() < 0) {
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = channel->events();
        pfd.revents = 0;
        pollfds_.push_back(pfd);
        int idx = pollfds_.size() - 1;
        channel->set_index(idx);
        channels_[pfd.fd] = channel;
    } else {
        int idx = channel->index();
        pollfds_[idx].events = channel->events();
        pollfds_[idx].revents = 0;
        if(channel->isNoneEvent()) {
            pollfds_[idx].fd = -1;
        }
    }
}

void Poller::removeChannel(Channel * channel) {
    assertInLoopThread();
    int idx = channel->index();
    if(static_cast<size_t>(idx) == pollfds_.size() - 1) {
        pollfds_.pop_back();
    } else {
        int channelAtEnd = pollfds_.back().fd;
        std::iter_swap(pollfds_.begin() + idx,pollfds_.end() - 1);
        if(channelAtEnd < 0) {
            channelAtEnd = -channelAtEnd - 1;
        }
        channels_[channelAtEnd]->set_index(idx);
        pollfds_.pop_back();
    }
}