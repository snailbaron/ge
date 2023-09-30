#include <gx/scene.hpp>

#include <cmath>
#include <utility>

#include <iostream>

namespace gx {

ScreenVector Camera::worldPointToScreenOffset(const WorldPoint& worldPosition) const
{
    return {
        .x = (worldPosition.x - position.x) * unitPixelSize * zoom,
        .y = (position.y - worldPosition.y) * unitPixelSize * zoom,
    };
}

WorldPoint Camera::screenOffsetToWorldPoint(const ScreenVector& offset) const
{
    return {
        .x = position.x + offset.x / unitPixelSize / zoom,
        .y = position.y - offset.y / unitPixelSize / zoom,
    };
}

void Camera::update(float delta)
{
    static constexpr float dragForce = 10.f;
    static constexpr float extraDrag = 1.f;

    if (follow) {
        auto toTarget = follow->position - position;
        float d = toTarget.length();
        float moveDistance = (dragForce * d * d + extraDrag) * delta;
        if (moveDistance >= d) {
            position = follow->position;
        } else {
            position += toTarget.resized(moveDistance);
        }
    }
}

Camera& Scene::camera()
{
    return _camera;
}

void Scene::update(float delta)
{
    _camera.update(delta);

    for (size_t i = 0; i < _objects.size(); ) {
        if (_objects.at(i)->kill) {
            std::swap(_objects.at(i), _objects.back());
            _objects.resize(_objects.size() - 1);
        } else {
            _objects.at(i)->animation.update(delta);
            i++;
        }
    }
}

void Scene::render(Renderer& renderer, const ScreenRectangle& area) const
{
    for (const auto& object : _objects) {
        auto objectOffset = _camera.worldPointToScreenOffset(object->position);
        auto objectPosition = area.middlePoint() + objectOffset;
        renderer.draw(
            object->animation.bitmap(),
            object->animation.frame(),
            objectPosition,
            (float)_camera.zoom);
    }
}

void Scene::setupCamera(
    const WorldPoint& center, float unitPixelSize, float zoom)
{
    _camera.position = center;
    _camera.unitPixelSize = unitPixelSize;
    _camera.zoom = zoom;
}

void Scene::cameraFollow(Object* object)
{
    _camera.follow = object;
}

Object* Scene::spawn(const Sprite& sprite, const WorldPoint& position)
{
    auto* ptr = new Object{
        .animation = Animation{sprite},
        .position = position
    };
    return _objects.emplace_back(ptr).get();
}

void Scene::clickAction(std::function<void(const WorldPoint&)> action)
{
    _clickAction = std::move(action);
}

Widget* Scene::locate(const ScreenRectangle& area, const ScreenPoint& point)
{
    return area.contains(point) ? this : nullptr;
}

void Scene::onPress(const ScreenRectangle& area, const ScreenPoint& point)
{
    if (_clickAction) {
        auto screenOffset = point - area.middlePoint();
        auto worldPoint = _camera.screenOffsetToWorldPoint(screenOffset);
        _clickAction(worldPoint);
    }
}

} // namespace gx
