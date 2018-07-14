//
// Created by rmqi on 12/7/18.
//

#include "Callbacks.h"
#include "TcpConnection.h"
#include "TcpServer.h"
#include <RN/base/ProcessInfo.h>

using namespace RN;
RN::string message;

void onConnection(const RN::TcpConnectionPtr &conn) {
    if (conn->connected()) {
        LOG_INFO << "new connection: " << conn->getPeerAddr().toHostPort()
                 << " - " << conn->getLocalAddr().toHostPort();
        conn->send(message);
    } else {
        printf("onConnection(): connection[%s] is down",
               conn->getName().c_str());
    }

}

void onWriteComplete(const RN::TcpConnectionPtr &conn) {
    conn->send(message);
}

void onMessage(const RN::TcpConnectionPtr &conn, RN::Buffer *data, RN::Timestamp receivedTime) {
    ssize_t readable = data->readableBytes();
    printf("onMessage: received %zd bytes from connection[%s], %s\n",
           readable, conn->getName().c_str(), data->retirveAllAsString().c_str());
}

int main() {
    printf("main(): pid = %d\n", getpid());
    RN::string line;
    for (int i = 33; i < 127; ++i) {
        line.push_back(char(i));
    }
    line += line;

    for (size_t i = 0; i < 127 - 33; ++i) {
        message += line.substr(i, 72);
        message.push_back('\n');
    }
    RN::InetAddress listenAddr(9981);
    RN::EventLoop loop;
    RN::TcpServer server("TestConnection", &loop, listenAddr);
    server.setMessageCallback(onMessage);
    server.setConnectionCallback(onConnection);
    server.setWriteCompleteCallback(onWriteComplete);
    server.start();
    loop.loop();

}