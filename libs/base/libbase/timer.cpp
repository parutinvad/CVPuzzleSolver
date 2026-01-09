#include "timer.h"

// Returns passed time (in seconds)
double Timer::elapsed() const noexcept {
    return std::chrono::duration<double>(Clock::now() - start_).count();
}

// Restarts timer measurements and returns elapsed time (in seconds) before reset
void Timer::restart() noexcept {
    const auto now = Clock::now();
    start_ = now;
}
