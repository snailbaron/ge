#pragma once

#include <gx/geometry.hpp>
#include <gx/renderer.hpp>
#include <gx/sprite.hpp>
#include <gx/ui_coordinate.hpp>

#include <SDL.h>

#include <concepts>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <iostream>

namespace gx {

struct UiTag;
using UiPoint = Point<UiCoordinate, UiTag>;
using UiVector = Vector<UiCoordinate, UiTag>;
using UiRectangle = Rectangle<UiCoordinate, UiTag>;

inline constexpr ScreenRectangle combine(
    const ScreenRectangle& outer, const UiRectangle& inner)
{
    return {
        .x = outer.x + inner.x.pixels + inner.x.fraction * outer.w,
        .y = outer.y + inner.y.pixels + inner.y.fraction * outer.h,
        .w = inner.w.pixels + inner.w.fraction * outer.w,
        .h = inner.h.pixels + inner.h.fraction * outer.h,
    };
}

inline constexpr ScreenRectangle combine(
    const ScreenRectangle& outer, const UiVector& vector)
{
    return {
        .x = vector.x.pixels + vector.x.fraction * outer.w,
        .y = vector.y.pixels + vector.y.fraction * outer.h,
    };
}

inline constexpr ScreenPoint combine(
    const ScreenRectangle& outer, const UiPoint& point)
{
    return {
        .x = outer.x + point.x.pixels + point.x.fraction * outer.w,
        .y = outer.y + point.y.pixels + point.y.fraction * outer.h,
    };
}

class Widget {
public:
    virtual ~Widget() = default;

    virtual void update(float /*delta*/) {}
    virtual void render(Renderer&, const ScreenRectangle&) const = 0;
    virtual Widget* locate(
        const ScreenRectangle& /*area*/, const ScreenPoint& /*point*/)
    {
        return nullptr;
    }

    virtual void onFocus() {}
    virtual void onUnfocus() {}
    virtual void onPress(const ScreenRectangle&, const ScreenPoint&) {}
    virtual void onRelease() {}
    virtual void onActivate() {}
    virtual void onDrag(const ScreenVector&) {}
};

class Button : public Widget {
public:
    Button* position(const UiCoordinate& x, const UiCoordinate& y)
    {
        _position = {x, y};
        return this;
    }

    Button* size(const UiCoordinate& w, const UiCoordinate& h)
    {
        _size = {w, h};
        return this;
    }

    Button* color(const Color& color)
    {
        _color = color;
        return this;
    }

    Button* buttonSprite(const Sprite& sprite)
    {
        _buttonAnimation = Animation{sprite};
        return this;
    }

    Button* pressedButtonSprite(const Sprite& sprite)
    {
        _pressedButtonAnimation = Animation{sprite};
        return this;
    }

    Button* textSprite(const Sprite& sprite)
    {
        _textAnimation = Animation{sprite};
        return this;
    }

    Button* action(std::function<void()> action)
    {
        _action = std::move(action);
        return this;
    }

    void render(Renderer& renderer, const ScreenRectangle& area) const override
    {
        auto screenPosition = combine(area, _position);

        if (buttonAnimation()) {
            buttonAnimation().draw(renderer, screenPosition);
        } else {
            auto screenArea = combine(area, uiArea());
            renderer.drawRectangle(screenArea, _color);
        }

        if (_textAnimation) {
            if (_pressed) {
                _textAnimation.draw(
                    renderer, screenPosition + ScreenVector{1, 1});
            } else {
                _textAnimation.draw(renderer, screenPosition);
            }
        }
    }

    Widget* locate(
        const ScreenRectangle& area, const ScreenPoint& point) override
    {
        auto screenArea = combine(area, uiArea());
        return screenArea.contains(point) ? this : nullptr;
    }

    void onPress(const ScreenRectangle&, const ScreenPoint&) override
    {
        _pressed = true;
    }

    void onRelease() override
    {
        _pressed = false;
    }

    void onActivate() override
    {
        _action();
    }

private:
    const Animation& buttonAnimation() const
    {
        return _pressed ? _pressedButtonAnimation : _buttonAnimation;
    }

    UiRectangle uiArea() const
    {
        if (auto animation = buttonAnimation(); animation) {
            auto pixelSize = animation.size();
            auto size = UiVector{
                .x = {.pixels = pixelSize.x},
                .y = {.pixels = pixelSize.y},
            };
            return UiRectangle::atPosition(_position, size);
        }

        return UiRectangle::atPosition(_position, _size);
    }

    UiPoint _position;
    UiVector _size;
    Color _color;
    Animation _buttonAnimation;
    Animation _pressedButtonAnimation;
    Animation _textAnimation;
    bool _pressed = false;
    std::function<void()> _action = []{};
};

class Panel : public Widget {
public:
    template <std::derived_from<Widget> T, class... Args>
    T* createWidget(Args&&... args)
    {
        _widgets.push_back(std::make_unique<T>(std::forward<Args>(args)...));
        return reinterpret_cast<T*>(_widgets.back().get());
    }

    void render(Renderer& renderer, const ScreenRectangle& area) const override
    {
        for (const auto& widget : _widgets) {
            widget->render(renderer, combine(area, _location));
        }
    }

private:
    std::vector<std::unique_ptr<Widget>> _widgets;
    UiRectangle _location;
};

} // namespace gx
