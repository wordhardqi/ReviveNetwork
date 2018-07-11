//
// Created by rmqi on 10/7/18.
//

#ifndef RN_IOEVENT_H
#define RN_IOEVENT_H

#include <unistd.h>
#include "Channel.h"

namespace RN {
    class IOWakeupTrigger {
    public:
        IOWakeupTrigger(EventLoop *loop);

        ~IOWakeupTrigger();

        void wakeup();

    private:
        void handleRead();

        EventLoop *loop_;
        int wakeupfd_;
        Channel wakeupChannel_;
    };
}

#endif //RN_IOEVENT_H
