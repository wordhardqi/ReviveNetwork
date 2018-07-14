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

void RN::TcpConnection::handleRead(Timestamp receivedTime) {
    int savedErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
    if (n > 0) {
        messageCallback_(shared_from_this(), &inputBuffer_, receivedTime);
    } else if (n == 0) {
        handleClose();
    } else if (n < 0) {
        errno = savedErrno;
        LOG_SYSERR << __FUNCTION__;
        handleError();
    }

}

void RN::TcpConnection::handleWrite() {
    loop_->assertInLoopThread();

    if (channel_->isWriting()) {
        ssize_t n = ::write(channel_->fd(),
                            outputBuffer_.peek(), outputBuffer_.readableBytes());
        if (n > 0) {
            outputBuffer_.retrive(n);
            if (outputBuffer_.readableBytes() == 0) {
                channel_->disableWriting();
                if (writeCompleteCallback_) {
                    loop_->runInLoop(
                            std::bind(writeCompleteCallback_, shared_from_this()));
                }
                if (state_ == kDisconnecting) {
                    shutdonwInLoop();
                }
            } else {
                LOG_TRACE << "Will write more data";
            }
        } else {
            LOG_SYSERR << __FUNCTION__;
        }
    } else {
        //FIXME :: shall I throw an error here?
        LOG_TRACE << "Connection is down, no more writting";
    }
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

void RN::TcpConnection::send(const RN::string &message) {
    if (state_ == kConnected) {
        if (loop_->isInLoopThread()) {
            sendInLoop(message);
        } else {
            loop_->runInLoop(std::bind(
                    &TcpConnection::sendInLoop, this, message));

        }
    }
}

void RN::TcpConnection::shutdown() {
    if (state_ == kConnected) {
        setState(kDisconnecting);
        loop_->runInLoop(
                std::bind(&TcpConnection::shutdonwInLoop, shared_from_this()));
    }
}

void RN::TcpConnection::sendInLoop(const RN::string &message) {
    loop_->assertInLoopThread();
    ssize_t nwrote = 0;
// outputBuffer_.readableBytes()==0 ==> nothing in outputBuffer
    if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
        nwrote = ::write(channel_->fd(), message.data(), message.size());
        if (nwrote < 0) {
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                LOG_SYSERR << __FUNCTION__;

            }

        } else {
            if (implicit_cast<size_t>(nwrote) < message.size()) {
                LOG_TRACE << "Will send more data";
            } else if (writeCompleteCallback_) {
                //write complete
                loop_->runInLoop(
                        std::bind(writeCompleteCallback_, shared_from_this()));
            }
        }

    }
    assert(nwrote >= 0);
    size_t remaining = message.size() - nwrote;
    if (remaining > 0) {
        size_t alreadInBuffer = outputBuffer_.readableBytes();
        if (alreadInBuffer < highWaterMark_ && alreadInBuffer + remaining >= highWaterMark_) {
            //triger highWaterMark callback
            loop_->queueInLoop(
                    std::bind(highWaterMarkCallback_, shared_from_this(), alreadInBuffer + remaining));
        }
        outputBuffer_.append(message.data() + nwrote, message.size() - nwrote);
        if (!channel_->isWriting()) {
            channel_->enableWriting();
        }
    }
}

void RN::TcpConnection::shutdonwInLoop() {
    loop_->assertInLoopThread();
    if (!channel_->isWriting()) {
        socket_->shutdownWrite();
    }
}



