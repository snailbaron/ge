#include <tempo/frame_timer.hpp>

#include <thread>

namespace tempo {

FrameTimer::FrameTimer(int fps)
    : _delta(1.0 / fps)
    , _lastFrame(0)
    , _startTime(Clock::now())
    , _frameDuration(std::chrono::duration_cast<Clock::duration>(
        std::chrono::duration<double>(_delta)))
{ }

double FrameTimer::delta() const
{
    return _delta;
}

int FrameTimer::operator()()
{
    auto currentTime = Clock::now();
    auto currentFrame = (currentTime - _startTime) / _frameDuration;
    auto frameDiff = currentFrame - _lastFrame;
    _lastFrame = currentFrame;
    return static_cast<int>(frameDiff);
}

void FrameTimer::relax() const
{
    auto nextFrameTime = _startTime + (_lastFrame + 1) * _frameDuration;
    std::this_thread::sleep_until(nextFrameTime);
}

} // namespace tempo
