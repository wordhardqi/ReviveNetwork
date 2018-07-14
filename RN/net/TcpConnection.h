//
// Created by rmqi on 11/7/18.
//

#ifndef RN_TCPCONNECITON_H
#define RN_TCPCONNECITON_H

#include <RN/base/Logging.h>
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Callbacks.h"
#include "Buffer.h"

namespace RN {
    class TcpConnection : RN::noncopyable,
                          public std::enable_shared_from_this<TcpConnection> {
    public:
        TcpConnection(EventLoop *loop,
                      const string &name,
                      int socketfd,
                      const InetAddress &localAddr,
                      const InetAddress &peerAddr)
                : loop_(loop), name_(name), state_(kConnecitng),
                  socket_(new Socket(socketfd)),
                  channel_(new Channel(loop_, socketfd)),
                  localAddr_(localAddr),
                  peerAddr_(peerAddr),
                  highWaterMark_(64 * 1024 * 1024) {
            LOG_DEBUG << "TcpConnection::ctor[" << name_
                      << "] at " << this << " fd=" << socketfd;
            channel_->setReadCallback(
                    std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
            channel_->setWriteCallback(
                    std::bind(&TcpConnection::handleWrite, this));
            channel_->setCloseCallback(
                    std::bind(&TcpConnection::handleClose, this));
            channel_->setErrorCallback(
                    std::bind(&TcpConnection::handleError, this));





        }

        ~TcpConnection() {
            LOG_DEBUG << "TcpConnection::dtor[" << name_ << "] at " << this
                      << " fd=" << channel_->fd();
        }


        EventLoop *getLoop() const {
            return loop_;
        }

        const string &getName() const {
            return name_;
        }

        const InetAddress &getLocalAddr() const {
            return localAddr_;
        }

        const InetAddress &getPeerAddr() const {
            return peerAddr_;
        }

        bool connected() const {
            return state_ == kConnected;
        }


        const Socket &getSocket() const {
            return *socket_;
        }

        const Channel &getChannel() const {
            return *channel_;
        }

        void setConnectionCallback(const ConnectionCallback &connectionCallback) {
            connectionCallback_ = connectionCallback;
        }

        void setMessageCallback(const MessageCallback &messageCallback) {
            messageCallback_ = messageCallback;
        }

        void setCloseCallback(const CloseCallback &cb) {
            closeCallback_ = cb;
        }

        void setHighWaterMarkCallback(const HighWaterMarkCallback &cb) {
            highWaterMarkCallback_ = cb;
        }

        void setWriteCompleteCallback(const WriteCompleteCallback &cb) {
            writeCompleteCallback_ = cb;
        }

        void connectionEstablished();

        void connectionDestroyed();

        void send(const string &message);

        void shutdown();

        void setTcpNoDelay(bool on) {

        }

        void setTcpKeepAlive(bool on) {

        }

    private:
        enum StateE {
            kConnecitng,
            kConnected,
            kDisconnecting,
            kDisconnected,

        };

        void handleRead(Timestamp);

        void handleWrite();

        void handleClose();

        void handleError();

        void setState(StateE s) {
            state_ = s;
        }

        void sendInLoop(const string &message);

        void shutdonwInLoop();


        EventLoop *loop_;
        string name_;

        StateE state_;
        std::unique_ptr<Socket> socket_;
        std::unique_ptr<Channel> channel_;
        InetAddress localAddr_;
        InetAddress peerAddr_;
        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        CloseCallback closeCallback_;
        WriteCompleteCallback writeCompleteCallback_;
        size_t highWaterMark_;
        HighWaterMarkCallback highWaterMarkCallback_;
        Buffer inputBuffer_;
        Buffer outputBuffer_;

    };
}


#endif //RN_TCPCONNECITON_H
