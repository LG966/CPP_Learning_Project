#pragma once

#include "aircraft_manager.hpp"
#include "airport.hpp"
#include "tower_sim.hpp"

class AircraftFactory
{
private:
    AircraftManager& _aircraftManager;
    Airport* _airport;
    std::vector<std::string> _airlines;
    std::vector<AircraftType*> _aircraft_types;
    std::unordered_set<std::string> used_flight_numbers {};

    const std::string get_flight_number()
    {
        while (true)
        {
            const std::string flight_number =
                _airlines[std::rand() % 8] + std::to_string(1000 + (rand() % 9000));
            if (used_flight_numbers.find(flight_number) == used_flight_numbers.end())
            {
                used_flight_numbers.emplace(flight_number);
                return flight_number;
            }
        }
    }

public:
    AircraftFactory(AircraftManager& aircraftManager, std::vector<std::string> airlines, Airport* airport) :
        _aircraftManager { aircraftManager }, _airport { airport }, _airlines { airlines }
    {}

    void create_aircraft(const AircraftType& type)
    {
        assert(_airport); // make sure the airport is initialized before creating aircraft

        std::cout << "HELLO" << std::endl;

        const std::string flight_number = get_flight_number();

        const float angle       = (rand() % 1000) * 2 * 3.141592f / 1000.f; // random angle between 0 and 2pi
        const Point3D start     = Point3D { std::sin(angle), std::cos(angle), 0 } * 3 + Point3D { 0, 0, 2 };
        const Point3D direction = (-start).normalize();

        Aircraft* aircraft = new Aircraft { type, flight_number, start, direction, _airport->get_tower() };
        // GL::display_queue.emplace_back(aircraft);
        _aircraftManager.add_aircraft(aircraft);
    }

    void create_random_aircraft() { create_aircraft(*(_aircraft_types[rand() % _aircraft_types.size()])); }

    void create_aircraft_type(const float max_ground_speed_, const float max_air_speed_,
                              const float max_accel_, const MediaPath& sprite)
    {
        _aircraft_types.emplace_back(
            new AircraftType { max_ground_speed_, max_air_speed_, max_accel_, sprite });
    }
};
