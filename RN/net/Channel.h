//
// Created by rmqi on 8/7/18.
//

#ifndef RN_CHANNEL_H
#define RN_CHANNEL_H

#include <RN/base/noncopyable.h>
#include <functional>

namespace RN {
    class EventLoop;

    class Channel : noncopyable {

    public:
        typedef std::function<void()> EventCallback;

        Channel(EventLoop *loop, int fd);

        void handleEvent();

        void setReadCallback(const EventCallback &cb) {
            readCallback_ = cb;
        }

        void setWriteCallback(const EventCallback &cb) {
            writeCallback_ = cb;
        }

        void setErrorCallback(const EventCallback &cb) {
            errorCallback_ = cb;
        }

        int fd() const {
            return fd_;
        }

        int events() const {
            return events_;
        }

        void set_revents(int revt) {
            revents_ = revt;
        }

        bool isNoneEvent() const {
            return events_ == kNoneEvent;
        }

        void enableReading() {
            events_ |= kReadEvent;
            update();
        }

        int index() {
            return index_;
        }

        void set_index(int idx) {
            index_ = idx;
        }

        EventLoop *ownerLoop() {
            return loop_;
        }

    private:
        void update();

        static const int kNoneEvent;
        static const int kReadEvent;
        static const int kWriteEvent;
        EventLoop *loop_;
        const int fd_;
        int events_;
        int revents_;
        int index_;
        EventCallback readCallback_;
        EventCallback writeCallback_;
        EventCallback errorCallback_;
    };
}


#endif //RN_CHANNEL_H
