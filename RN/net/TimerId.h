//
// Created by rmqi on 8/7/18.
//

#ifndef RN_TIMERID_H
#define RN_TIMERID_H

#include <RN/base/copyable.h>
#include <memory>
#include "Timer.h"

namespace RN {
    class Timer;

    class TimerId : public RN::copyable {
    public:
        explicit TimerId(TimerPtr timer)
                : value_(timer) {
        }

    private:
        TimerPtr value_;
    };
}
#endif //RN_TIMERID_H
