#pragma once

#include "aircraft.hpp"

#include <algorithm>
#include <vector>

class AircraftManager : public GL::DynamicObject
{

private:
    std::vector<std::unique_ptr<Aircraft>> _aircrafts;
    unsigned int crash_count = 0;

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
                      if (a1->has_terminal() && a2->has_terminal())
                      {
                          return a2->fuel > a1->fuel;
                      }
                      if (a1->has_terminal())
                          return true;
                      if (a2->has_terminal())
                          return false;
                      else
                          return a2->fuel > a1->fuel;
                  });

        for (auto& aircraft : _aircrafts)
        {
            try
            {
                aircraft->move();
            } catch (const AircraftCrash& err)
            {
                aircraft->_to_delete = true;
                std::cerr << err.what() << std::endl;
                crash_count++;
            }
        }

        destroy_aircrafts();
    }

    void add_aircraft(std::unique_ptr<Aircraft>& aircraft) { _aircrafts.emplace_back(std::move(aircraft)); }

    int count_aircraft_with_airline(const std::string& airline) const
    {
        auto is_from_airline = [airline](const std::unique_ptr<Aircraft>& aircraft)
        { return (*aircraft).get_flight_num().rfind(airline, 0) != std::string::npos; };
        return std::count_if(_aircrafts.begin(), _aircrafts.end(), is_from_airline);
    }

    unsigned int get_required_fuel() const
    {
        unsigned int total = 0;
        for (const auto& aircraft : _aircrafts)
        {
            if (aircraft->is_low_on_fuel() && aircraft->_is_at_terminal())
            {
                total += 3000 - aircraft->fuel;
            }
        }
        return total;
    }

    unsigned int get_crash_count() const { return crash_count; }
};