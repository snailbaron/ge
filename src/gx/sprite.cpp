#include <gx/sprite.hpp>

#include <gx/error.hpp>

#include <algorithm>
#include <cmath>

#include <iostream>

namespace gx {

Sprite createSimpleSprite(const Bitmap& bitmap, int frameCount, float fps)
{
    auto size = bitmap.size();

    if (size.x % frameCount != 0) {
        throw Error{
            "cannot divide texture of width " + std::to_string(size.x) +
            " into " + std::to_string(frameCount) + " parts"};
    }
    auto spriteWidth = size.x / frameCount;

    auto sprite = Sprite{};
    for (int i = 0; i < frameCount; i++) {
        sprite.frames.push_back(SpriteFrame{
            .bitmap = &bitmap,
            .frame = PixelRectangle{
                .x = i * spriteWidth,
                .y = 0,
                .w = spriteWidth,
                .h = size.y
            },
            .duration = 1.f / fps,
        });
    }

    return sprite;
}

Sprite createOneFrameSprite(
    const Bitmap& bitmap, const PixelRectangle& frame, float zoom)
{
    auto fittedFrame = frame;
    fittedFrame.x = std::max(fittedFrame.x, 0);
    fittedFrame.y = std::max(fittedFrame.y, 0);
    fittedFrame.w = std::min(fittedFrame.w, bitmap.size().x);
    fittedFrame.h = std::min(fittedFrame.h, bitmap.size().y);

    auto sprite = Sprite{.zoom = zoom};
    sprite.frames.push_back(SpriteFrame{
        .bitmap = &bitmap,
        .frame = fittedFrame,
        .duration = 1.f
    });

    return sprite;
}

Animation::Animation(const Sprite& sprite)
    : _sprite(&sprite)
{
    _durationSum.reserve(sprite.frames.size());
    float sum = 0.0;
    for (const auto& frame : sprite.frames) {
        sum += frame.duration;
        _durationSum.push_back(sum);
    }
}

const Bitmap& Animation::bitmap() const
{
    return *_sprite->frames.at(_frameIndex).bitmap;
}

const PixelRectangle& Animation::frame() const
{
    return _sprite->frames.at(_frameIndex).frame;
}

ScreenVector Animation::size() const
{
    const auto& f = frame();
    return {(float)f.w * _sprite->zoom, (float)f.h * _sprite->zoom};
}

void Animation::update(float delta)
{
    float totalDuration = _durationSum.back();
    _time = std::fmod(_time + delta, totalDuration);
    _frameIndex =
        std::ranges::upper_bound(_durationSum, _time) - _durationSum.begin();
}

void Animation::draw(Renderer& renderer, const ScreenPoint& position) const
{
    renderer.draw(bitmap(), frame(), position, _sprite->zoom);
}

void Animation::noloop()
{
    _loop = false;
}

} // namespace gx
