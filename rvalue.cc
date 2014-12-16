#include <iostream>

int twice(int& a) {
    std::cout << "lvalue (int&)" << std::endl;
    return (a++) * 2;
}

int twice(const int& a) {
    std::cout << "const lvalue (const int&)" << std::endl;
    return a * 2;
}

int twice(int&& a) {
    std::cout << "rvalue (int&&)" << std::endl;
    a++;
    return a * 2;
}

template <typename T>
void t_twice(T&& t) {
    std::cout << "(T&&)" << t << std::endl;
    t = twice(std::forward<T>(t));
}

int main(int argc, char *argv[]) {
    int a = 5;
    const int b = 3;
    twice(a); // lvalue
    std::cout << a << std::endl;
    twice(b); // const lvalue
    twice(5); // rvalue
    twice(twice(a)); // rvalue(lvalue)
    std::cout << a << std::endl;
    twice(a + a); // rvalue
    twice(static_cast<int&&>(a)); // rvalue
    twice(std::move(a)); // rvalue
    std::cout << a << std::endl;
    // http://cpplover.blogspot.jp/2009/11/rvalue-reference_23.html
    // template関数の引数にrvalue referenceを使った場合、
    // lvalue を渡すとlvalue reference になると言う特別ルールがあるらしい。
    t_twice(5); // rvalue T = int&&
    t_twice(a); // lvalue T = int&
    std::cout << a << std::endl;
    return 0;
}
