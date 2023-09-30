#pragma once

#include <gx/geometry.hpp>

#include <SDL.h>
#include <SDL_ttf.h>

#include <chrono>
#include <filesystem>
#include <memory>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace gx {

struct ScreenTag;
using ScreenVector = Vector<float, ScreenTag>;
using ScreenPoint = Point<float, ScreenTag>;
using ScreenRectangle = Rectangle<float, ScreenTag>;

struct PixelTag;
using PixelVector = Vector<int, PixelTag>;
using PixelPoint = Point<int, PixelTag>;
using PixelRectangle = Rectangle<int, PixelTag>;

class Bitmap {
public:
    Bitmap();

    PixelVector size() const;

private:
    explicit Bitmap(SDL_Texture* ptr);

    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> _ptr;

    friend class Renderer;
};

class Cursor {
public:
    Cursor();

private:
    explicit Cursor(SDL_Surface* ptr, int x, int y);

    std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)> _surface;
    std::unique_ptr<SDL_Cursor, void(*)(SDL_Cursor*)> _cursor;

    friend class Renderer;
};

struct Color {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;
};

class Font {
public:
    Font() = default;
    Font(const std::filesystem::path& path, int ptSize);

private:
    std::unique_ptr<TTF_Font, void(*)(TTF_Font*)> _ptr {nullptr, TTF_CloseFont};

    friend class Renderer;
};

class Renderer {
public:
    Renderer();

    Bitmap loadBitmap(const std::filesystem::path& path) const;
    Bitmap loadBitmap(const std::span<const std::byte>& data) const;

    static Cursor loadCursor(const std::filesystem::path& path, int x, int y);
    static void setCursor(Cursor& cursor);

    Bitmap prepareText(
        const Font& font,
        const std::string& text,
        const Color& color,
        int maxLength = 0);

    void draw(
        const Bitmap& bitmap,
        const PixelRectangle& frame,
        const ScreenPoint& position,
        float zoom = 1.f);

    void drawRectangle(const ScreenRectangle& rectangle, const Color& color);

    bool processEvent(const SDL_Event& e);
    void clear();
    void present();

    const ScreenVector& windowSize() const;
    ScreenRectangle windowArea() const;

private:
    ScreenVector _windowSize;
    std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> _window;
    std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> _renderer;
};

} // namespace gx
