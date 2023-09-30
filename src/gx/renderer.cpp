#include <gx/renderer.hpp>

#include <gx/error.hpp>

#include <SDL_image.h>

#include <utility>

#include <iostream>

namespace gx {

namespace {

} // namespace

Bitmap::Bitmap()
    : _ptr(nullptr, SDL_DestroyTexture)
{ }

Bitmap::Bitmap(SDL_Texture* ptr)
    : _ptr(ptr, SDL_DestroyTexture)
{ }

Cursor::Cursor()
    : _surface(nullptr, SDL_FreeSurface)
    , _cursor(nullptr, SDL_FreeCursor)
{ }

Cursor::Cursor(SDL_Surface* ptr, int x, int y)
    : _surface(ptr, SDL_FreeSurface)
    , _cursor(
        sdlCheck(SDL_CreateColorCursor(_surface.get(), x, y)),
        SDL_FreeCursor)
{ }

PixelVector Bitmap::size() const
{
    auto size = PixelVector{};
    sdlCheck(SDL_QueryTexture(_ptr.get(), nullptr, nullptr, &size.x, &size.y));
    return size;
}

Font::Font(const std::filesystem::path& path, int ptSize)
{
    _ptr.reset(sdlCheck(TTF_OpenFont(path.string().c_str(), ptSize)));
}

Renderer::Renderer()
    : _window(
        sdlCheck(SDL_CreateWindow(
            "gx",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            1024,
            768,
            SDL_WINDOW_RESIZABLE)),
        SDL_DestroyWindow)
    , _renderer(
        sdlCheck(SDL_CreateRenderer(
            _window.get(),
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)),
        SDL_DestroyRenderer)
{
    int x = 0;
    int y = 0;
    SDL_GetWindowSize(_window.get(), &x, &y);
    _windowSize = {(float)x, (float)y};
}

Bitmap Renderer::loadBitmap(const std::filesystem::path& path) const
{
    return Bitmap{
        sdlCheck(IMG_LoadTexture(_renderer.get(), path.string().c_str()))
    };
}

Bitmap Renderer::loadBitmap(const std::span<const std::byte>& data) const
{
    return Bitmap{
        sdlCheck(IMG_LoadTexture_RW(
            _renderer.get(),
            sdlCheck(SDL_RWFromMem(
                (void*)data.data(),
                static_cast<int>(data.size()))),
            1 /* freesrc */))
    };
}

Cursor Renderer::loadCursor(const std::filesystem::path& path, int x, int y)
{
    return Cursor{sdlCheck(IMG_Load(path.string().c_str())), x, y};
}

void Renderer::setCursor(Cursor& cursor)
{
    SDL_SetCursor(cursor._cursor.get());
}

Bitmap Renderer::prepareText(
    const Font& font,
    const std::string& text,
    const Color& color,
    int maxLength)
{
    auto sdlColor = SDL_Color{color.r, color.g, color.b, color.a};

    SDL_Surface* surface = nullptr;
    if (maxLength == 0) {
        surface = TTF_RenderUTF8_Blended(
            font._ptr.get(), text.c_str(), sdlColor);
    } else {
        surface = TTF_RenderUTF8_Blended_Wrapped(
            font._ptr.get(), text.c_str(), sdlColor, maxLength);
    }
    if (!surface) {
        throw Error{"cannot render text to surface"};
    }

    auto bitmap = Bitmap{sdlCheck(
        SDL_CreateTextureFromSurface(_renderer.get(), surface))};

    SDL_FreeSurface(surface);

    return bitmap;
}

void Renderer::draw(
    const Bitmap& bitmap,
    const PixelRectangle& frame,
    const ScreenPoint& position,
    float zoom)
{
    auto src = SDL_Rect{.x = frame.x, .y = frame.y, .w = frame.w, .h = frame.h};
    auto dst = SDL_FRect{
        .x = position.x - zoom * (float)frame.w / 2.f,
        .y = position.y - zoom * (float)frame.h / 2.f,
        .w = zoom * (float)frame.w,
        .h = zoom * (float)frame.h
    };

    sdlCheck(SDL_RenderCopyF(_renderer.get(), bitmap._ptr.get(), &src, &dst));
}

void Renderer::drawRectangle(
    const ScreenRectangle& rectangle, const Color& color)
{
    auto rect = SDL_FRect{
        .x = rectangle.x,
        .y = rectangle.y,
        .w = rectangle.w,
        .h = rectangle.h,
    };

    sdlCheck(SDL_SetRenderDrawColor(
        _renderer.get(), color.r, color.g, color.b, color.a));
    sdlCheck(SDL_RenderFillRectF(_renderer.get(), &rect));
}

bool Renderer::processEvent(const SDL_Event& e)
{
    if (e.type == SDL_WINDOWEVENT &&
            e.window.event == SDL_WINDOWEVENT_RESIZED) {
        _windowSize = {(float)e.window.data1, (float)e.window.data2};
        return true;
    }

    return false;
}

void Renderer::clear()
{
    sdlCheck(SDL_SetRenderDrawColor(_renderer.get(), 0, 0, 0, 255));
    sdlCheck(SDL_RenderClear(_renderer.get()));
}

void Renderer::present()
{
    SDL_RenderPresent(_renderer.get());
}

const ScreenVector& Renderer::windowSize() const
{
    return _windowSize;
}

ScreenRectangle Renderer::windowArea() const
{
    return {0, 0, _windowSize.x, _windowSize.y};
}

} // namespace gx
