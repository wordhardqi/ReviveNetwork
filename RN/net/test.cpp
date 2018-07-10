#include "EventLoop.h"
#include <RN/base/Thread.h>
#include <RN/base/ProcessInfo.h>

RN::EventLoop *g_loop;
int cnt = 0;

void printTid() {
    printf("pid = %d, tid = %d\n", getpid(), RN::CurrentThread::tid());
    printf("now %s\n", RN::Timestamp::now().toString().c_str());
}

void print(const char *msg) {
    printf("msg %s %s\n", RN::Timestamp::now().toString().c_str(), msg);
    if (++cnt == 20) {
        g_loop->quit();
    }
}

int main() {
    printTid();
    RN::EventLoop loop;
    g_loop = &loop;
    print("main");
    loop.runAfter(1, std::bind(print, "once 1"));
    loop.runAfter(1.5, std::bind(print, "once 1.5"));
    loop.runAfter(2.5, std::bind(print, "once 2.5"));
    loop.runAfter(3.5, std::bind(print, "once 3.5"));
    loop.runEvery(2, std::bind(print, "every 2"));
    loop.runEvery(3, std::bind(print, "every 3"));
    loop.loop();
    print("main loop exited");


}
