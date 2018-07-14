//
// Created by rmqi on 8/7/18.
//

#ifndef RN_CHANNEL_H
#define RN_CHANNEL_H

#include <RN/base/noncopyable.h>
#include <functional>
#include <RN/base/Timestamp.h>
namespace RN {
    class EventLoop;


    class Channel : noncopyable {

    public:
        typedef std::function<void()> EventCallback;
        typedef std::function<void(Timestamp)> ReadEventCallback;

        Channel(EventLoop *loop, int fd);

        ~Channel();

        void handleEvent(Timestamp);

        void setReadCallback(const ReadEventCallback &cb) {
            readCallback_ = cb;
        }

        void setWriteCallback(const EventCallback &cb) {
            writeCallback_ = cb;
        }

        void setErrorCallback(const EventCallback &cb) {
            errorCallback_ = cb;
        }

        void setCloseCallback(const EventCallback &cb) {
            closeCallback_ = cb;
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

        void enableWriting() {
            events_ |= kWriteEvent;
            update();
        }

        void disableWriting() {
            events_ &= ~kWriteEvent;
            update();
        }

        bool isWriting() {
            return events_ & kWriteEvent;
        }


        void disableAll() {
            events_ = kNoneEvent;
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
        bool eventHandling_;

        ReadEventCallback readCallback_;
        EventCallback writeCallback_;
        EventCallback errorCallback_;
        EventCallback closeCallback_;
    };
}


#endif //RN_CHANNEL_H
