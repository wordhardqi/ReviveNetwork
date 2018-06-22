//
// Created by renming on 6/19/18.
//

#ifndef ECHO_NONCOPYABLE_H
#define ECHO_NONCOPYABLE_H
namespace RN {
class noncopyable {
 protected:
  noncopyable() = default;
  ~noncopyable() = default;
 private:
  noncopyable(const noncopyable &) = delete;
  void operator=(const noncopyable &) = delete;
};
}
#endif //ECHO_NONCOPYABLE_H
