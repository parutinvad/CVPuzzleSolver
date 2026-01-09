#pragma once

#include <chrono>

class Timer {
public:
    // Constructor starts timer measurements
    Timer() noexcept { restart(); }

    // Returns passed time (in seconds)
    double elapsed() const noexcept;

    // Restarts timer measurements and returns elapsed time (in seconds) before reset
    void restart() noexcept;

private:
#if defined(_WIN32)
    // On Windows, steady_clock is typically backed by QueryPerformanceCounter (high-res, monotonic)
    using Clock = std::chrono::steady_clock;
#elif defined(__APPLE__)
    // On macOS, steady_clock is monotonic; high-resolution in practice
    using Clock = std::chrono::steady_clock;
#else
    // On Linux, steady_clock is monotonic; usually backed by CLOCK_MONOTONIC
    using Clock = std::chrono::steady_clock;
#endif

    Clock::time_point start_{};
};
