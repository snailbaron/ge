#include <gx/box.hpp>

#include <gx/error.hpp>

#include <SDL_image.h>

#include <ranges>
#include <string>

namespace gx {

Box::Box()
{
    sdlCheck(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS));
    sdlCheck(IMG_Init(IMG_INIT_PNG));
    sdlCheck(TTF_Init());
}

Box::~Box()
{
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

Bitmap Box::loadBitmap(const std::filesystem::path& path) const
{
    return _renderer.loadBitmap(path);
}

Bitmap Box::loadBitmap(const std::span<const std::byte>& data) const
{
    return _renderer.loadBitmap(data);
}

Cursor Box::loadCursor(const std::filesystem::path& path, int x, int y)
{
    return Renderer::loadCursor(path, x, y);
}

void Box::setCursor(Cursor& cursor)
{
    Renderer::setCursor(cursor);
}

void Box::update(float delta)
{
    for (const auto& widget : _widgets) {
        widget->update(delta);
    }
}

void Box::present()
{
    _renderer.clear();
    for (const auto& widget : _widgets) {
        widget->render(_renderer, _renderer.windowArea());
    }
    _renderer.present();
}

bool Box::alive() const
{
    return _alive;
}

bool Box::dead() const
{
    return !_alive;
}

Renderer& Box::renderer()
{
    return _renderer;
}

bool Box::processEvent(const SDL_Event& e)
{
    if (e.type == SDL_QUIT) {
        _alive = false;
        return true;
    }
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
        _alive = false;
        return true;
    }

    processUiEvent(e) || _renderer.processEvent(e);

    return false;
}

Widget* Box::widgetAtPosition(int x, int y) const
{
    const auto& windowArea = _renderer.windowArea();
    auto point = ScreenPoint{(float)x, (float)y};
    for (const auto& widget : _widgets | std::views::reverse) {
        if (auto* target = widget->locate(windowArea, point)) {
            return target;
        }
    }
    return nullptr;
}

bool Box::processUiEvent(const SDL_Event& e)
{
    auto windowArea = _renderer.windowArea();

    if (e.type == SDL_MOUSEMOTION) {
        auto* newFocusedWidget = widgetAtPosition(e.motion.x, e.motion.y);
        if (newFocusedWidget != _focusedWidget) {
            if (_focusedWidget) {
                _focusedWidget->onUnfocus();
            }
            _focusedWidget = newFocusedWidget;
            if (_focusedWidget &&
                    (!_pressedWidget || _focusedWidget == _pressedWidget)) {
                _focusedWidget->onFocus();
            }
        }
        if (_pressedWidget) {
            auto drag =
                ScreenVector{(float)e.motion.xrel, (float)e.motion.yrel};
            _pressedWidget->onDrag(drag);
        }
        return true;
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (_focusedWidget) {
            auto point = ScreenPoint{(float)e.button.x, (float)e.button.y};
            _focusedWidget->onPress(windowArea, point);
            _pressedWidget = _focusedWidget;
            return true;
        }
    } else if (e.type == SDL_MOUSEBUTTONUP) {
        if (_pressedWidget) {
            _pressedWidget->onRelease();
            if (_pressedWidget == _focusedWidget) {
                _pressedWidget->onActivate();
            } else if (_focusedWidget) {
                _focusedWidget->onFocus();
            }
            _pressedWidget = nullptr;
            return true;
        }
    }

    return false;
}

} // namespace gx
