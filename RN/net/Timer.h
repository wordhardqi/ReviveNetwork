//
// Created by rmqi on 8/7/18.
//

#ifndef RN_TIMER_H
#define RN_TIMER_H

#include <RN/base/Timestamp.h>
#include <bits/unique_ptr.h>
#include <memory>
#include "Callbacks.h"

namespace RN {
    class Timer {
    public:
        Timer(const TimerCallback &cb, Timestamp when, double interval)
                : callback_(cb),
                  expiration_(when),
                  interval_(interval),
                  repeat_(interval > 0.0) {

        }

        void run() const {
            callback_();
        }

        bool repeat() const {
            return repeat_;
        }

        void restart(Timestamp now);

        Timestamp expiration() {
            return expiration_;
        }

    private:
        const TimerCallback callback_;
        Timestamp expiration_;
        const double interval_;
        bool repeat_;
    };

    typedef std::shared_ptr<Timer> TimerPtr;
}

#endif //RN_TIMER_H
