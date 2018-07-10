//
// Created by rmqi on 8/7/18.
//

#include "Timer.h"

void RN::Timer::restart(RN::Timestamp now) {
    if (repeat_) {
        expiration_ = addTime(now, interval_);
    } else {
        expiration_ = Timestamp::invalid();
    }

}
