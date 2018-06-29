//
// Created by rmqi on 28/6/18.
//

#include <string.h>
#include <cstdio>
#include <string>
#include <iostream>
#include <limits>

using namespace std;

class Foo {
public:
    Foo(int val) : value_(val) {}

    int value_;
};

ostream &operator<<(ostream &o, const Foo foo) {
    o << foo.value_;
    return o;
}

static const int kMaxNumbericSize = 32;


int main() {


    const char *x = "abc";
    printf("%d", static_cast<int>(strlen(x)));
    Foo foo(1);
    cout << foo;
}