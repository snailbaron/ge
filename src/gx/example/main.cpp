#include <gx.hpp>

#include <SDL.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <map>
#include <ostream>
#include <thread>

#include <iostream>

using namespace gx::literals;

std::filesystem::path executableDirectory()
{
#ifdef __linux__
    return std::filesystem::read_symlink("/proc/self/exe").parent_path();
#endif
}

struct Vector;

constexpr float length(const Vector& vector);
constexpr Vector normalize(const Vector& vector);
constexpr float distance(const Vector& lhs, const Vector& rhs);

struct Vector {
    constexpr Vector& operator+=(const Vector& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr Vector& operator-=(const Vector& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr Vector& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    constexpr Vector& operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    constexpr void resize(float newSize)
    {
        if (auto l = length(*this); l != 0) {
            *this *= newSize / l;
        }
    }

    constexpr Vector resized(float newSize) const
    {
        auto copy = *this;
        copy.resize(newSize);
        return copy;
    }

    constexpr bool zero() const
    {
        return x == 0 && y == 0;
    }

    float x = 0.f;
    float y = 0.f;
};

std::ostream& operator<<(std::ostream& output, const Vector& vector)
{
    return output << "(" << vector.x << ", " << vector.y << ")";
}

constexpr Vector operator+(Vector lhs, const Vector& rhs)
{
    lhs += rhs;
    return lhs;
}

constexpr Vector operator-(Vector lhs, const Vector& rhs)
{
    lhs -= rhs;
    return lhs;
}

constexpr Vector operator*(Vector lhs, float scalar)
{
    lhs *= scalar;
    return lhs;
}

constexpr Vector operator*(float scalar, Vector lhs)
{
    lhs *= scalar;
    return lhs;
}

constexpr Vector operator/(Vector lhs, float scalar)
{
    lhs /= scalar;
    return lhs;
}

constexpr float length(const Vector& vector)
{
    return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}

constexpr Vector normalize(const Vector& vector)
{
    if (float l = length(vector); l != 0) {
        return vector / l;
    }
    return {.x = 0, .y = 0};
}

constexpr float distance(const Vector& lhs, const Vector& rhs)
{
    return length(lhs - rhs);
}

struct KeyboardControl {
    bool moveLeft = false;
    bool moveRight = false;
    bool moveUp = false;
    bool moveDown = false;
};

enum class ObjectType {
    None,
    Grass,
    Road,
    Tree,
    Stone,
    Hero,
    Bullet,
};

struct Object {
    size_t id = 0;
    ObjectType type = ObjectType::Grass;
    Vector position;
};

struct Bullet {
    size_t id = 0;
    Vector position;
    Vector velocity;
    float age = 0.f;
};

struct Message {
    size_t objectId = 0;
    ObjectType type = ObjectType::None;
    bool alive = true;
    float x = 0.f;
    float y = 0.f;
};

// Poor man's event queue
std::vector<Message> messages;

struct World {
    void initialize()
    {
        constexpr auto G = ObjectType::Grass;
        constexpr auto R = ObjectType::Road;
        constexpr auto T = ObjectType::Tree;
        constexpr auto S = ObjectType::Stone;
        using Row = std::array<ObjectType, 11>;

        constexpr auto data = std::array<Row, 11>{
            Row{T, T, T, T, T, R, T, T, T, T, T},
            Row{T, G, G, G, G, R, G, G, G, G, T},
            Row{T, G, G, S, G, R, G, G, G, G, T},
            Row{T, G, G, G, G, R, R, G, G, G, T},
            Row{T, G, G, G, G, G, R, G, G, G, T},
            Row{T, G, G, T, G, G, R, G, G, G, T},
            Row{T, G, G, G, G, G, R, R, R, G, T},
            Row{T, G, G, G, G, S, G, G, R, G, T},
            Row{T, G, G, G, G, G, G, G, R, G, T},
            Row{T, G, G, G, G, G, G, G, R, G, T},
            Row{T, T, T, T, T, T, T, T, R, T, T},
        };

        for (int i = 0; i < 11; i++) {
            for (int j = 0; j < 11; j++) {
                const auto& type = data.at(i).at(j);
                if (type == ObjectType::Tree || type == ObjectType::Stone) {
                    float x = (float)j - 5.f;
                    float y = 5.f - (float)i;
                    auto object = Object{
                        .id = nextId++,
                        .type = data.at(i).at(j),
                        .position = {x, y}
                    };
                    objects.push_back(object);
                    messages.push_back(Message{
                        .objectId = object.id,
                        .type = object.type,
                        .x = object.position.x,
                        .y = object.position.y,
                    });
                }
            }
        }

    }

    void update(float delta)
    {
        constexpr static float timeToSpeedup = 0.3f;
        constexpr static float timeToStop = 0.2;
        constexpr static float maxSpeed = 5.f;
        constexpr static float acceleration = maxSpeed / timeToSpeedup;
        constexpr static float deceleration = maxSpeed / timeToStop;

        auto controlAcceleration = controlVector();

        heroVelocity += controlAcceleration * (acceleration + deceleration) * delta;
        if (auto l = length(heroVelocity); l != 0) {
            float newLength = l - deceleration * delta;
            newLength = std::clamp(newLength, 0.f, maxSpeed);
            heroVelocity *= newLength / l;
        }

        heroPosition += heroVelocity * delta;

        for (const auto& object : objects) {
            if (float d = distance(heroPosition, object.position); d < 1) {
                auto pushVector = heroPosition - object.position;
                pushVector.resize(1 - d);
                heroPosition += pushVector;
            }
        }

        for (size_t i = 0; i < bullets.size(); ) {
            auto& bullet = bullets.at(i);
            bullet.position += bullet.velocity * delta;
            bullet.age += delta;
            messages.push_back(Message{
                .objectId = bullet.id,
                .x = bullet.position.x,
                .y = bullet.position.y});

            bool collision = false;
            for (size_t j = 0; j < objects.size(); ) {
                auto& object = objects.at(j);
                if (distance(bullet.position, object.position) < 0.6f) {
                    collision = true;
                    messages.push_back(Message{
                        .objectId = object.id, .alive = false});
                    std::swap(object, objects.back());
                    objects.resize(objects.size() - 1);
                } else {
                    j++;
                }
            }

            if (collision || bullet.age > 1.f) {
                messages.push_back(
                    Message{.objectId = bullet.id, .alive = false});
                std::swap(bullet, bullets.back());
                bullets.resize(bullets.size() - 1);
            } else {
                i++;
            }
        }
    }

    Vector controlVector() const
    {
        auto controlVector = Vector{};
        if (control.moveLeft) {
            controlVector.x -= 1.f;
        }
        if (control.moveRight) {
            controlVector.x += 1.f;
        }
        if (control.moveUp) {
            controlVector.y += 1.f;
        }
        if (control.moveDown) {
            controlVector.y -= 1.f;
        }
        return normalize(controlVector);
    }

    bool processEvent(const SDL_Event& e)
    {
        if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
            bool press = (e.type == SDL_KEYDOWN);
            if (e.key.keysym.sym == SDLK_a) {
                control.moveLeft = press;
                return true;
            }
            if (e.key.keysym.sym == SDLK_d) {
                control.moveRight = press;
                return true;
            }
            if (e.key.keysym.sym == SDLK_w) {
                control.moveUp = press;
                return true;
            }
            if (e.key.keysym.sym == SDLK_s) {
                control.moveDown = press;
                return true;
            }
        }
        return false;
    }

    void shootInDirectionOf(const Vector& targetPoint)
    {
        static constexpr float bulletSpeed = 10.f;

        auto bulletVelocity = (targetPoint - heroPosition).resized(bulletSpeed);
        if (!bulletVelocity.zero()) {
            bullets.push_back(Bullet{
                .id = nextId++,
                .position = heroPosition,
                .velocity = bulletVelocity});
            messages.push_back(Message{
                .objectId = bullets.back().id,
                .type = ObjectType::Bullet,
                .x = bullets.back().position.x,
                .y = bullets.back().position.y,
            });
        }
    }

    Vector heroPosition;
    Vector heroVelocity;
    KeyboardControl control;
    std::vector<Object> objects;
    std::vector<Bullet> bullets;
    size_t nextId = 0;
};

struct Client {
    KeyboardControl keyboardControl;
};

struct Resources {
    struct Bitmaps {
        gx::Bitmap grass;
        gx::Bitmap tree;
        gx::Bitmap hero;
        gx::Bitmap stone;
        gx::Bitmap button;
        gx::Bitmap pressAnimation;
        gx::Bitmap quitTextBitmap;
        gx::Bitmap bullet;
    };

    struct Sprites {
        gx::Sprite grass;
        gx::Sprite tree;
        gx::Sprite hero;
        gx::Sprite stone;
        gx::Sprite buttonNormal;
        gx::Sprite buttonPressed;
        gx::Sprite pressAnimation;
        gx::Sprite quitTextSprite;
        gx::Sprite bullet;
    };

    struct Fonts {
        gx::Font main;
    };

    Bitmaps bitmaps;
    Sprites sprites;
    Fonts fonts;
    gx::Cursor cursor;
};

Resources loadResources(gx::Box& box)
{
    auto r = Resources{};

    const auto root = executableDirectory();

    r.bitmaps.grass = box.loadBitmap(root / "grass.png");
    r.sprites.grass = gx::createSimpleSprite(r.bitmaps.grass, 2, 3);

    r.bitmaps.tree = box.loadBitmap(root / "tree.png");
    r.sprites.tree = gx::createSimpleSprite(r.bitmaps.tree, 2, 3);

    r.bitmaps.hero = box.loadBitmap(root / "hero.png");
    r.sprites.hero = gx::createSimpleSprite(r.bitmaps.hero);

    r.bitmaps.stone = box.loadBitmap(root / "stone.png");
    r.sprites.stone = gx::createSimpleSprite(r.bitmaps.stone);

    r.bitmaps.bullet = box.loadBitmap(root / "bullet.png");
    r.sprites.bullet = gx::createSimpleSprite(r.bitmaps.bullet);

    r.cursor = gx::Box::loadCursor(root / "cursor.png", 2, 0);

    r.bitmaps.button = box.loadBitmap(root / "button.png");
    r.sprites.buttonNormal =
        gx::createOneFrameSprite(r.bitmaps.button, {0, 0, 64, 16}, 3);
    r.sprites.buttonPressed =
        gx::createOneFrameSprite(r.bitmaps.button, {0, 16, 64, 16}, 3);

    r.bitmaps.pressAnimation = box.loadBitmap(root / "press-animation.png");
    r.sprites.pressAnimation =
        gx::createSimpleSprite(r.bitmaps.pressAnimation, 7, 14);

    r.fonts.main = gx::Font{root / "nasalization-rg.otf", 18};
    r.bitmaps.quitTextBitmap = box.renderer().prepareText(
        r.fonts.main, "Quit", {0, 0, 0, 255});
    r.sprites.quitTextSprite = gx::createSimpleSprite(r.bitmaps.quitTextBitmap);

    return r;
}

class Timer {
    using Clock = std::chrono::high_resolution_clock;

public:
    Timer(int fps)
        : _delta(1.f / (float)fps)
        , _frameDuration(std::chrono::duration_cast<Clock::duration>(
            std::chrono::duration<float>(_delta)))
        , _start(Clock::now())
    { }

    float delta() const
    {
        return _delta;
    }

    int operator()()
    {
        auto now = Clock::now();
        size_t frame = (now - _start) / _frameDuration;
        int framesPassed = static_cast<int>(frame - _lastFrame);
        _lastFrame = frame;
        return framesPassed;
    }

    void relax() const
    {
        std::this_thread::sleep_until(
            _start + _frameDuration * (_lastFrame + 1));
    }

private:
    float _delta = 0.f;
    Clock::duration _frameDuration;
    Clock::time_point _start;
    size_t _lastFrame = 0;
};

int main()
{
    auto world = World{};
    world.initialize();

    auto box = gx::Box{};
    auto r = loadResources(box);
    gx::Box::setCursor(r.cursor);

    auto* scene = box.createWidget<gx::Scene>();

    bool quitRequested = false;

    box.createWidget<gx::Button>()
        ->position(1_fr - 20_px - 32_px * 3, 20_px + 8_px * 3)
        ->color({100, 50, 50})
        ->buttonSprite(r.sprites.buttonNormal)
        ->pressedButtonSprite(r.sprites.buttonPressed)
        ->textSprite(r.sprites.quitTextSprite)
        ->action([&quitRequested] { quitRequested = true; });

    std::map<size_t, gx::Object*> objects;

    auto* hero = scene->spawn(r.sprites.hero, gx::WorldPoint{0, 0});
    scene->setupCamera(gx::WorldPoint{0, 0}, 16, 4);
    scene->cameraFollow(hero);
    scene->clickAction([&world] (const gx::WorldPoint& point) {
        world.shootInDirectionOf({point.x, point.y});
    });

    static constexpr int fps = 60;
    auto timer = Timer{fps};

    for (;;) {
        for (SDL_Event e; SDL_PollEvent(&e); ) {
            box.processEvent(e) || world.processEvent(e);
        }
        if (box.dead() || quitRequested) {
            break;
        }

        if (int framesPassed = timer(); framesPassed > 0) {
            // Update world with the same delta for perfect reproducibility
            for (int i = 0; i < framesPassed; i++) {
                world.update((float)timer.delta());
            }

            for (const auto& message : messages) {
                if (auto it = objects.find(message.objectId);
                        it != objects.end()) {
                    auto* object = it->second;
                    if (!message.alive) {
                        object->kill = true;
                        objects.erase(it);
                    } else {
                        object->position = {message.x, message.y};
                    }
                } else {
                    gx::Sprite* sprite = nullptr;
                    switch (message.type) {
                        case ObjectType::Stone:
                            sprite = &r.sprites.stone;
                            break;
                        case ObjectType::Tree:
                            sprite = &r.sprites.tree;
                            break;
                        case ObjectType::Hero:
                            sprite = &r.sprites.hero;
                            break;
                        case ObjectType::Bullet:
                            sprite = &r.sprites.bullet;
                            break;
                        default:
                            break;
                    }

                    if (sprite) {
                        auto* object = scene->spawn(
                            *sprite, {message.x, message.y});
                        objects.emplace(message.objectId, object);
                    }
                }
            }
            messages.clear();

            hero->position = {world.heroPosition.x, world.heroPosition.y};

            box.update(timer.delta() * (float)framesPassed);
            box.present();
        }

        timer.relax();
    }
}
