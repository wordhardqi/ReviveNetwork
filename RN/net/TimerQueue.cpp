//
// Created by rmqi on 8/7/18.
//

#include <sys/timerfd.h>
#include <RN/base/Logging.h>
#include "TimerQueue.h"

namespace RN {
    namespace detail {
        int createTimerfd() {
            int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
            if (timerfd < 0) {
                LOG_SYSFATAL << "Failed in timerfd_create";
            }
            return timerfd;
        }

        struct timespec getTimeFromNow(Timestamp when) {
            int64_t microseconds = when.microSecondsSinceEpoch()
                                   - Timestamp::now().microSecondsSinceEpoch();

            //TODO :: Reason for doing so.
            if (microseconds < 100) {
                microseconds = 100;
            }
            struct timespec ts;
            ts.tv_sec = static_cast<time_t >(
                    microseconds / Timestamp::kMicroSecondsPerSecond);
            ts.tv_nsec = static_cast<long>(
                    microseconds % Timestamp::kMicroSecondsPerSecond);
            return ts;
        }

        void resetTimerfd(int timerfd, Timestamp expiration) {
            struct itimerspec new_value;
            struct itimerspec old_value;
            bzero(&new_value, sizeof(new_value));
            bzero(&old_value, sizeof(old_value));
            new_value.it_value = getTimeFromNow(expiration);
            int ret = timerfd_settime(timerfd, 0, &new_value, &old_value);
            if (ret) {
                LOG_SYSERR << "timerfd_set()";
            }

        }

        void readTimerfd(int timerfd, Timestamp now) {
            uint64_t howmany;
            ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
            LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
            assert(n == sizeof(howmany));
        }
    }
}

RN::TimerQueue::TimerQueue(RN::EventLoop *loop)
        : loop_(loop),
          timerfd_(createTimerfd()),
          timerfdChannel_(loop, timerfd_),
          timers_() {
    timerfdChannel_.setReadCallback(
            std::bind(&TimerQueue::handleRead, this));
    timerfdChannel_.enableReading();

}

RN::TimerQueue::~TimerQueue() {
    ::close(timerfd_);
}

bool RN::TimerQueue::insert(RN::TimerPtr timer) {
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    TimerList::iterator it = timers_.begin();
    if (it == timers_.end() || when < it->first) {
        earliestChanged = true;
    }
    auto result = timers_.insert(std::make_pair(when, timer));
    assert(result.second);
    return earliestChanged;

}

bool RN::TimerQueue::remove(RN::TimerId timerToRemove) {
    bool earliestChanged = false;
    TimerId firstTimer = TimerId(timers_.begin()->second);
    if (firstTimer == timerToRemove) {
        timers_.erase()
    }
}

RN::TimerId RN::TimerQueue::addTimer(const RN::TimerCallback &cb,
                                     RN::Timestamp when,
                                     double interval) {
    TimerPtr newTimer(new Timer(cb, when, interval));
//    loop_->assertInLoopThread();
//    bool earliestChanged = insert(newTimer);
//    if (earliestChanged) {
//        resetTimerfd(timerfd_, when);
//    }
//    return TimerId(newTimer);
    loop_->runInLoop(
            std::bind(&TimerQueue::addTimerInLoop, this, newTimer));
    return TimerId(newTimer);

}

void RN::TimerQueue::addTimerInLoop(RN::TimerPtr timer) {
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);
    if (earliestChanged) {
        resetTimerfd(timerfd_, timer->expiration());
    }

}

void RN::TimerQueue::handleRead() {
    loop_->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd_, now);
    std::vector<Entry> expired = getExpired(now);
    for (auto &x : expired) {
        x.second->run();
    }
    reset(expired, now);


}

std::vector<RN::TimerQueue::Entry> RN::TimerQueue::getExpired(RN::Timestamp now) {
    Entry guard = std::make_pair(nextMicroSeconds(now), TimerPtr());
    TimerList::iterator it = timers_.lower_bound(guard);
    assert(it == timers_.end() || it->first > now);

    std::vector<Entry> expired(
            std::make_move_iterator(timers_.begin()), std::make_move_iterator(it));
    timers_.erase(timers_.begin(), it);
    return expired;

}

void RN::TimerQueue::reset(const std::vector<RN::TimerQueue::Entry> &expired, RN::Timestamp now) {
    for (auto &t : expired) {
        if (t.second->repeat()) {
            t.second->restart(now);
            insert(t.second);
        }
    }
    Timestamp nextExpire;
    if (!timers_.empty()) {
        nextExpire = timers_.begin()->second->expiration();
    }
    if (nextExpire.valid()) {
        resetTimerfd(timerfd_, nextExpire);
    }

}
