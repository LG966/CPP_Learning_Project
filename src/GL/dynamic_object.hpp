#pragma once

#include <unordered_set>

namespace GL {

class DynamicObject
{
public:
    bool _to_delete = false;
    virtual ~DynamicObject() {}

    virtual void move() = 0;
};

inline std::unordered_set<DynamicObject*> move_queue;

} // namespace GL
