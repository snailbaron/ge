#pragma once

namespace tempo {

class Metronome {
public:
    explicit Metronome(int fps = 1);

    void reset(int fps);
    int ticks(double delta);

private:
    double _period = 0.0;
    double _offset = 0.0;
};

} // namespace tempo
