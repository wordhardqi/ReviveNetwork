//
// Created by rmqi on 28/6/18.
//

#include <vector>
#include <iostream>

using namespace std;

class Foo {
public:
    Foo(int val) : value_(val) {

    }

    int value_;
};

int main() {
    Foo foo1(1);
    Foo foo2(2);
    vector<Foo> fv;
    fv.push_back(std::move(foo1));
    fv.push_back(foo2);
    fv[0].value_ = 2;
    cout << "fool" << foo1.value_ << endl;
    cout << "foo2" << foo2.value_ << endl;
}