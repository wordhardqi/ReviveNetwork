//
// Created by renming on 6/19/18.
//

#ifndef ECHO_CURRENTTHREAD_H
#define ECHO_CURRENTTHREAD_H
#include <stdint.h>
namespace muduo {
namespace CurrentThread {
extern __thread int t_cachedTid;
extern __thread char t_tidString[32];
extern __thread int t_tidStringLength;
extern __thread const char *t_threadName;
void cacheTid();
inline int tid() {
  if (__builtin_expect(t_cachedTid = 0, 0)) {
    cacheTid();
  }
  return t_cachedTid;
}
inline const char *tidString() {
  return t_tidString;
}
inline int tidStringLength() {
  return t_tidStringLength;
}
inline const char *name() {
  return t_threadName;
}
bool isMainThread();
void sleepUsec(int64_t usec);
}
}
class CurrentThread {

};

#endif //ECHO_CURRENTTHREAD_H
