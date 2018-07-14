//
// Created by rmqi on 13/7/18.
//

#ifndef RN_CONNECTOR_H
#define RN_CONNECTOR_H

#include "Channel.h"
#include "Callbacks.h"
#include "InetAddress.h"

namespace RN {

    class EventLoop;

    class Connector {

    public:
        Connector(EventLoop *loop, const InetAddress &serverAddr);

        ~Connector();

        void connect();

        void connecting();

        void handleWrite();

        void handleError();

        const static int kRetryDelayMs = 1000;
    private:
        enum StateE {
            kDisconnected, kConnected, kConnecting
        };
        EventLoop *loop_;
        InetAddress serverAddr_;
        std::unique_ptr<Channel> channel_;
        bool connect_;
        StateE state_;
        ConnectionCallback connectionCallback_;


    };
}


#endif //RN_CONNECTOR_H
