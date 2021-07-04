#include "mineutils.h"
#include <iostream>

void CHECK(bool x, const char* msg = "ERROR") {
    if (!x) {
        std::cout << msg << std::endl;
        std::terminate();
    }
}

void valid() {
    std::cout << "\tEnter valid testcase..." << std::endl;
    using Holy::Point;
    CHECK(Point{ 1, 1 }.valid(), "1 1");
    CHECK(Point{ 30, 16 }.valid(), "30 16");
    CHECK(!Point{ 0, 0 }.valid(), "0 0");
    CHECK(!Point{ 31, 12 }.valid(), "31 12");
}

void nei4() {
    std::cout << "\tEnter nei4 testcase..." << std::endl;
    using namespace Holy;
    GameData a;
    Point{ 10, 10 }.for_each_nei8([&](Point np) { a[np].label = 7; });
    CHECK(a[{ 9, 9 }].label == 7, "9 9");
    CHECK(a[{ 10, 10 }].label == 0, "10 10");
}

int main() {
    using namespace Holy;
    std::cout << "Running test cases for mineutils..." << std::endl;
    valid();
    nei4();
    std::cout << "Success" << std::endl;
}