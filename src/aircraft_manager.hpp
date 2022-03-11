#pragma once

#include "aircraft.hpp"

#include <vector>

class AircraftManager : public GL::DynamicObject
{

private:
    std::vector<std::unique_ptr<Aircraft>> _aircrafts;

    void destroy_aircrafts()
    {
        auto is_to_delete = [](const std::unique_ptr<Aircraft>& aircraft) { return (*aircraft)._to_delete; };

        _aircrafts.erase(std::remove_if(_aircrafts.begin(), _aircrafts.end(), is_to_delete),
                         _aircrafts.end());
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

    void add_aircraft(std::unique_ptr<Aircraft>& aircraft) { _aircrafts.emplace_back(std::move(aircraft)); }
};