//
// Created by rmqi on 10/7/18.
//

#include "IOWakeupTrigger.h"
#include <sys/eventfd.h>
#include <RN/base/Logging.h>

namespace RN {
    namespace detail {
        static int createEventfd() {
            int fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
            if (fd < 0) {
                LOG_SYSERR << "Failed in eventfd";
                abort();
            }
            return fd;
        }
    }
}

RN::IOWakeupTrigger::IOWakeupTrigger(RN::EventLoop *loop)
        : loop_(loop),
          wakeupfd_(createEventfd()),
          wakeupChannel_(loop, wakeupfd_) {
    wakeupChannel_.setWriteCallback(
            std::bind(&IOWakeupTrigger::handleRead, this));
    wakeupChannel_.enableReading();
}

RN::IOWakeupTrigger::~IOWakeupTrigger() {
    ::close(wakeupfd_);
}

void RN::IOWakeupTrigger::wakeup() {
    uint64_t dummy = 1;
    ssize_t n = ::write(wakeupfd_, &dummy, sizeof(dummy));
    if (n != sizeof(dummy)) {
        LOG_ERROR << "IOWakeupTrigger::wakeup() writes " << n << " bytes instead of 8";
    }

}

void RN::IOWakeupTrigger::handleRead() {
    uint64_t dummy = 1;
    ssize_t n = ::read(wakeupfd_, &dummy, sizeof(dummy));
    if (n != sizeof(dummy)) {
        LOG_ERROR << "IOWakeupTrigger::handleRead() writes " << n << " bytes instead of 8";
    }
}
