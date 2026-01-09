#include "timer.h"

#include <gtest/gtest.h>

#include <iostream>

TEST(timer, justTryIt) {
    Timer t;
    std::cout << "test finished in " << t.elapsed() << " sec" << std::endl;
}
