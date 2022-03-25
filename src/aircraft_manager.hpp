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
        // sort aircrafts : has a terminal -> low on fuel -> high on fuel

        std::sort(_aircrafts.begin(), _aircrafts.end(),
                  [](const std::unique_ptr<Aircraft>& a1, const std::unique_ptr<Aircraft>& a2)
                  {
                      if (a2->has_terminal() || a2->fuel < a1->fuel)
                          return true;
                      else
                          return false;
                  });

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

    unsigned int get_required_fuel()
    {
        unsigned int total = 0;
        for (const auto& aircraft : _aircrafts)
        {
            if (aircraft->is_low_on_fuel() && aircraft->has_terminal())
            {
                total += aircraft->fuel;
            }
        }
        return total;
    }
};