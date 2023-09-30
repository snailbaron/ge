#pragma once

#include <gx/renderer.hpp>

#include <vector>

namespace gx {

struct SpriteFrame {
    const Bitmap* bitmap = nullptr;
    PixelRectangle frame;
    float duration = 0.f;
};

struct Sprite {
    std::vector<SpriteFrame> frames;
    float zoom = 1.f;
};

Sprite createSimpleSprite(
    const Bitmap& bitmap, int frameCount = 1, float fps = 1.f);

Sprite createOneFrameSprite(
    const Bitmap& bitmap, const PixelRectangle& frame, float zoom = 1.f);

class Animation {
public:
    Animation() = default;
    explicit Animation(const Sprite& sprite);

    explicit operator bool() const
    {
        return _sprite != nullptr;
    }

    const Bitmap& bitmap() const;
    const PixelRectangle& frame() const;
    ScreenVector size() const;

    void update(float delta);
    void draw(Renderer& renderer, const ScreenPoint& position) const;

    void noloop();

private:
    const Sprite* _sprite = nullptr;
    std::vector<float> _durationSum;
    float _time = 0.0;
    size_t _frameIndex = 0;
    bool _loop = true;
};

} // namespace gx
