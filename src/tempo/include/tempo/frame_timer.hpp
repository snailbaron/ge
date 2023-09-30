#pragma once

#include <chrono>

namespace tempo {

class FrameTimer {
public:
    explicit FrameTimer(int fps);
    [[nodiscard]] double delta() const;
    int operator()();
    void relax() const;

private:
    using Clock = std::chrono::high_resolution_clock;

    const double _delta;
    long long _lastFrame;
    Clock::time_point _startTime;
    Clock::duration _frameDuration;
};

} // namespace tempo
