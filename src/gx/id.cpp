#include <gx/id.hpp>

namespace gx {

Id IdPool::create()
{
    if (!_free.empty()) {
        auto id = _free.front();
        _free.pop();
        return id;
    }

    auto id = _nextId;
    _nextId = Id{_nextId + 1};
    return id;
}

void IdPool::kill(Id id)
{
    _free.push(id);
}

} // namespace gx
