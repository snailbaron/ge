#pragma once

#include <gx/geometry.hpp>
#include <gx/id.hpp>
#include <gx/renderer.hpp>
#include <gx/sprite.hpp>
#include <gx/ui.hpp>

#include <chrono>
#include <cmath>
#include <functional>
#include <map>
#include <vector>

namespace gx {

struct WorldTag;
using WorldVector = Vector<float, WorldTag>;
using WorldPoint = Point<float, WorldTag>;

struct Object {
    Animation animation;
    WorldPoint position;
    bool kill = false;
};

struct Camera {
    ScreenVector worldPointToScreenOffset(const WorldPoint& worldPosition) const;
    WorldPoint screenOffsetToWorldPoint(const ScreenVector& offset) const;
    void update(float delta);

    WorldPoint position;
    float unitPixelSize = 1.f;
    float zoom = 1.f;
    Object* follow = nullptr;
};

class Scene : public Widget {
public:
    Camera& camera();

    void update(float delta) override;
    void render(Renderer& renderer, const ScreenRectangle& area) const override;

    void setupCamera(const WorldPoint& center, float unitPixelSize, float zoom);
    void cameraFollow(Object* object);

    Object* spawn(const Sprite& sprite, const WorldPoint& position);

    void clickAction(std::function<void(const WorldPoint&)> action);

    Widget* locate(
        const ScreenRectangle& area, const ScreenPoint& point) override;

    void onPress(
        const ScreenRectangle& area, const ScreenPoint& point) override;

private:
    Camera _camera;
    std::vector<std::unique_ptr<Object>> _objects;
    std::function<void(const WorldPoint&)> _clickAction;
};

} // namespace gx
