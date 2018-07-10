//
// Created by rmqi on 8/7/18.
//

#include <poll.h>
#include <RN/base/Logging.h>
#include "Channel.h"
#include "EventLoop.h"

namespace RN {
    const int Channel::kNoneEvent = 0;
    const int Channel::kReadEvent = POLLIN | POLLPRI;
    const int Channel::kWriteEvent = POLLOUT;
}

void RN::Channel::update() {
    loop_->updateChannel(this);
}

RN::Channel::Channel(RN::EventLoop *loop, int fd)
        : loop_(loop),
          fd_(fd),
          events_(0), revents_(0), index_(-1) {

}

void RN::Channel::handleEvent() {
    if (revents_ & POLLNVAL) {
        LOG_WARN << "Channel::handle_event() POLLNVAL";
    }
    if (revents_ & (POLLERR | POLLNVAL)) {
        if (errorCallback_) errorCallback_();
    }
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (readCallback_) readCallback_();
    }
    if (revents_ & POLLOUT) {
        if (writeCallback_) writeCallback_();
    }

}
