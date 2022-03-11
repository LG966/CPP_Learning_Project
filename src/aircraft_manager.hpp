#pragma once

#include "aircraft.hpp"

#include <algorithm>
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

    int count_aircraft_with_airline(const std::string& airline)
    {
        auto is_from_airline = [airline](const std::unique_ptr<Aircraft>& aircraft)
        { return (*aircraft).get_flight_num().rfind(airline, 0) != std::string::npos; };
        return std::count_if(_aircrafts.begin(), _aircrafts.end(), is_from_airline);
    }
};