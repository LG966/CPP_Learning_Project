#pragma once

#include "aircraft.hpp"

class AircraftManager : public GL::DynamicObject
{

private:
    std::unordered_set<std::unique_ptr<Aircraft>> _aircrafts;

    void destroy_aircrafts()
    {
        auto it = _aircrafts.begin();
        while (it != _aircrafts.end())
        {
            if ((*it)->_to_delete)
            {
                it = _aircrafts.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

public:
    void move()
    {
        for (auto& aircraft : _aircrafts)
        {
            aircraft->move();
        }

        destroy_aircrafts();
    }

    void add_aircraft(Aircraft* aircraft) { _aircrafts.emplace(aircraft); }
};