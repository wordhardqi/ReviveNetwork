//
// Created by rmqi on 11/7/18.
//

#include "TcpConnection.h"

void RN::TcpConnection::connectionEstablished() {
    loop_->assertInLoopThread();
    assert(state_ == kConnecitng);
    setState(kConnected);
    channel_->enableReading();
    connectionCallback_(shared_from_this());
}

void RN::TcpConnection::connectionDestroyed() {
    assert(state_ == kConnected);
    setState(kDisconnected);
    channel_->disableAll();
    connectionCallback_(shared_from_this());
    loop_->removeChannle(channel_.get());
}

void RN::TcpConnection::handleRead() {
    char buf[65536];
    ssize_t n = ::read(channel_->fd(), buf, sizeof(buf));
    if (n > 0) {
        messageCallback_(shared_from_this(), buf, n);
    } else if (n == 0) {
        handleClose();
    } else {
        handleError();
    }

}

void RN::TcpConnection::handleWrite() {

}

void RN::TcpConnection::handleClose() {
    loop_->assertInLoopThread();
    LOG_TRACE << "TcpConnection::handleClose state = " << state_;
    assert(state_ = kConnected);
    channel_->disableAll();
    closeCallback_(shared_from_this());

}

void RN::TcpConnection::handleError() {
    int err = sockets::getSocketError(channel_->fd());
    LOG_ERROR << "TcpConnection::handleError [" << name_
              << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}
