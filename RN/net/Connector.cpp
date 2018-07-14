//

#include "Connector.h"

//
// Created by rmqi on 13/7/18.
RN::Connector::Connector(RN::EventLoop *loop, const RN::InetAddress &serverAddr)
        : loop_(loop), serverAddr_(serverAddr), connect_(false), state_(kDisconnected) {

}


RN::Connector::~Connector() {

}

void RN::Connector::connect() {

}

void RN::Connector::connecting() {

}

void RN::Connector::handleWrite() {

}

void RN::Connector::handleError() {

}
