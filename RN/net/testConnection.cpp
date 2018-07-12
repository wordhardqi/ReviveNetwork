//
// Created by rmqi on 12/7/18.
//

#include "Callbacks.h"
#include "TcpConnection.h"
#include "TcpServer.h"
#include <RN/base/ProcessInfo.h>

void onConnection(const RN::TcpConnectionPtr &conn) {
    if (conn->connected()) {
        printf("onConnection(): new connection[%s] from %s \n",
               conn->getName().c_str(), conn->getPeerAddr().toHostPort().c_str());
    } else {
        printf("onConnection(): connection[%s] is down",
               conn->getName().c_str());
    }
}

void onMessage(const RN::TcpConnectionPtr &conn, const char *data, ssize_t len) {
    printf("onMessage: received %zd bytes from connection[%s], %s\n",
           len, conn->getName().c_str(), data);
}

int main() {
    printf("main(): pid = %d\n", getpid());
    RN::InetAddress listenAddr(9981);
    RN::EventLoop loop;
    RN::TcpServer server("TestConnection", &loop, listenAddr);
    server.setMessageCallback(onMessage);
    server.setConnectionCallback(onConnection);
    server.start();
    loop.loop();

}