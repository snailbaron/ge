#pragma once

#include <map>
#include <queue>
#include <utility>

namespace gx {

class Id {
public:
    using Base = unsigned long;

    explicit Id(Base id) : _id(id) {}
    operator Base() const { return _id; }

private:
    Base _id = 0;
};

class IdPool {
public:
    Id create();
    void kill(Id id);

private:
    Id _nextId {1};
    std::queue<Id> _free;
};

template <class Id, class Object>
class ObjectCacheIterator {
public:
    explicit ObjectCacheIterator(typename std::map<Id, Object>::iterator base)
        : _base(base)
    { }

    Object& operator*() const
    {
        return _base->second;
    }

    ObjectCacheIterator& operator++()
    {
        ++_base;
        return *this;
    }

    ObjectCacheIterator operator++(int)
    {
        auto temp = *this;
        ++*this;
        return temp;
    }

    auto operator<=>(const ObjectCacheIterator&) const = default;

private:
    typename std::map<Id, Object>::iterator _base;
};

template <class Id, class Object>
class ObjectCacheConstIterator {
public:
    explicit ObjectCacheConstIterator(
        typename std::map<Id, Object>::const_iterator base)
        : _base(base)
    { }

    const Object& operator*() const
    {
        return _base->second;
    }

    ObjectCacheConstIterator& operator++()
    {
        ++_base;
        return *this;
    }

    ObjectCacheConstIterator operator++(int)
    {
        auto temp = *this;
        ++*this;
        return temp;
    }

    auto operator<=>(const ObjectCacheConstIterator&) const = default;

private:
    typename std::map<Id, Object>::const_iterator _base;
};

template <class Id, class Object>
class ObjectCache {
public:
    Id push(Object&& object)
    {
        auto id = Id{_ids.create()};
        _objects.emplace(id, std::move(object));
        return id;
    }

    template <class... Args>
    Id emplace(Args&&... args)
    {
        auto id = Id{_ids.create()};
        _objects.emplace(id, Object{std::forward<Args>(args)...});
        return id;
    }

    void pop(Id id)
    {
        _objects.erase(id);
        _ids.kill(id);
    }

    Object& operator[](Id id)
    {
        return _objects.at(id);
    }

    const Object& operator[](Id id) const
    {
        return _objects.at(id);
    }

    ObjectCacheIterator<Id, Object> begin()
    {
        return ObjectCacheIterator<Id, Object>{_objects.begin()};
    }

    ObjectCacheConstIterator<Id, Object> begin() const
    {
        return ObjectCacheConstIterator<Id, Object>{_objects.begin()};
    }

    ObjectCacheIterator<Id, Object> end()
    {
        return ObjectCacheIterator<Id, Object>{_objects.end()};
    }

    ObjectCacheConstIterator<Id, Object> end() const
    {
        return ObjectCacheConstIterator<Id, Object>{_objects.end()};
    }

private:
    std::map<Id, Object> _objects;
    IdPool _ids;
};

} // namespace gx
