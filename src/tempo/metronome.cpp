#include <tempo/metronome.hpp>

#include <cmath>

namespace tempo {

Metronome::Metronome(int fps)
{
    reset(fps);
}

void Metronome::reset(int fps)
{
    _period = 1.0 / fps;
    _offset = 0;
}

int Metronome::ticks(double delta)
{
    _offset += delta;
    auto ticks = static_cast<int>(_offset / _period);
    _offset = fmod(_offset, _period);
    return ticks;
}

} // namespace tempo
