#pragma once

#include <iostream>
#include <vector>

namespace GL {

// a displayable object can be displayed and has a z-coordinate indicating who
// is displayed before whom ;]

class Displayable;

inline std::vector<const Displayable*> display_queue;

class Displayable
{
protected:
    float z = 0;

public:
    Displayable(const float z_) : z { z_ } {}
    virtual ~Displayable()
    {
        auto dq_it = display_queue.begin();
        while (dq_it != display_queue.end())
        {
            if (*dq_it == this)
            {
                dq_it = display_queue.erase(dq_it);
            }
            else
            {
                ++dq_it;
            }
        }
    }

    virtual void display() const = 0;

    float get_z() const { return z; }
};

struct disp_z_cmp
{
    bool operator()(const Displayable* a, const Displayable* b) const
    {
        const auto az = a->get_z();
        const auto bz = b->get_z();
        return (az == bz) ? (a > b) : (az > bz);
    }
};

} // namespace GL
