//
// Created by renming on 6/21/18.
//
#include <string>
#include <RN/base/CurrentThread.h>
#include <RN/base/Thread.h>
#include <RN/base/ThreadLocalSingleton.h.h>
#include <stdio.h>
#include <unistd.h>

using std::string;
class Test {
 public:
  Test() {
    printf("tid: %d construction %p", CurrentThread::tid(), this);
  }
  ~Test() {
    printf("tid: %d destroying %p %s", CurrentThread::tid(), this, name_.c_str());
  }
  void set(string change_to) {
    name_ = change_to;
  }
  const string &name() { return name_; }
 private:
  string name_;
};
#define STL RN::ThreadLocalSingleton<Test>::instance()
void print() {
  printf("tid: %d print %p, %s",
         CurrentThread::tid(), &STL,
         STL.name().c_str());

}
void thread_func(string change_to) {
  print();
  STL.set(change_to);
  sleep(1);
  print();
}
int main() {
  STL.set("main one");
  RN::Thread thread_1(std::bind(thread_func, "thread_1"));
  RN::Thread thread_2(std::bind(thread_func, "thread_2"));
  thread_1.start();
  thread_2.start();
  print();
  thread_1.join();
  thread_2.join();
  pthread_exit(0);

}