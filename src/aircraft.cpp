#include "aircraft.hpp"

#include "GL/opengl_interface.hpp"

#include <algorithm>
#include <cmath>

void Aircraft::turn_to_waypoint()
{
    if (!waypoints.empty())
    {
        Point3D target = waypoints[0];
        if (waypoints.size() > 1)
        {
            const float d   = (waypoints[0] - pos).length();
            const Point3D W = (waypoints[0] - waypoints[1]).normalize(d / 2.0f);
            target += W;
        }

        turn(target - pos - speed);
    }
}

void Aircraft::turn(Point3D direction)
{
    (speed += direction.cap_length(type.max_accel)).cap_length(max_speed());
}

unsigned int Aircraft::get_speed_octant() const
{
    const float speed_len = speed.length();
    if (speed_len > 0)
    {
        const Point3D norm_speed { speed * (1.0f / speed_len) };
        const float angle =
            (norm_speed.y() > 0) ? 2.0f * 3.141592f - std::acos(norm_speed.x()) : std::acos(norm_speed.x());
        // partition into NUM_AIRCRAFT_TILES equal pieces
        return (static_cast<int>(std::round((angle * NUM_AIRCRAFT_TILES) / (2.0f * 3.141592f))) + 1) %
               NUM_AIRCRAFT_TILES;
    }
    else
    {
        return 0;
    }
}

// when we arrive at a terminal, signal the tower
void Aircraft::arrive_at_terminal()
{
    // we arrived at a terminal, so start servicing
    control.arrived_at_terminal(*this);
    is_at_terminal = true;
}

// deploy and retract landing gear depending on next waypoints
void Aircraft::operate_landing_gear()
{
    if (waypoints.size() > 1u)
    {
        const auto it            = waypoints.begin();
        const bool ground_before = it->is_on_ground();
        const bool ground_after  = std::next(it)->is_on_ground();
        // deploy/retract landing gear when landing/lifting-off
        if (ground_before && !ground_after)
        {
            std::cout << flight_number << " lift off" << std::endl;
            this->add_waypoint<false>(Waypoint { Point3D {}, wp_destroy });
        }
        else if (!ground_before && ground_after)
        {
            std::cout << flight_number << " is now landing..." << std::endl;
            landing_gear_deployed = true;
        }
        else if (!ground_before && !ground_after)
        {
            landing_gear_deployed = false;
        }
    }
}

void Aircraft::move()
{
    assert(fuel >= 0);

    if (!waypoints.empty() && waypoints.back().type == wp_destroy)
    {
        _to_delete = true;
        return;
    }

    if (this->is_circling())
    {
        WaypointQueue wp = control.reserve_terminal(*this);
        if (!wp.empty())
        {
            waypoints = std::move(wp);
        }
    }

    if (waypoints.empty())
    {
        constexpr auto front = false;
        for (const auto& wp : control.get_instructions(*this))
        {
            add_waypoint<front>(wp);
        }
    }

    if (!is_at_terminal)
    {
        turn_to_waypoint();
        // move in the direction of the current speed
        pos += speed;

        // if we are close to our next waypoint, stike if off the list
        if (!waypoints.empty() && distance_to(waypoints.front()) < DISTANCE_THRESHOLD)
        {
            if (waypoints.front().is_at_terminal())
            {
                arrive_at_terminal();
            }
            else
            {
                operate_landing_gear();
            }
            waypoints.pop_front();
        }

        if (is_on_ground())
        {
            if (!landing_gear_deployed)
            {
                using namespace std::string_literals;
                throw AircraftCrash { flight_number + " crashed into the ground"s };
            }
        }
        else
        {
            fuel--;
            if (fuel == 0)
            {
                // cancel terminal reservation in case the aircraft was heading to a terminal
                control.cancel_reservation_terminal(*this);
                throw AircraftCrash { flight_number + " is crashing !" };
            }
            // if we are in the air, but too slow, then we will sink!
            const float speed_len = speed.length();
            if (speed_len < SPEED_THRESHOLD)
            {
                pos.z() -= SINK_FACTOR * (SPEED_THRESHOLD - speed_len);
            }
        }

        // update the z-value of the displayable structure
        GL::Displayable::z = pos.x() + pos.y();
    }
}

void Aircraft::display() const
{
    type.texture.draw(project_2D(pos), { PLANE_TEXTURE_DIM, PLANE_TEXTURE_DIM }, get_speed_octant());
}

bool Aircraft::has_terminal() const
{
    if (!waypoints.empty())
    {
        return this->waypoints.back().type == WaypointType::wp_terminal;
    }
    return false;
}

bool Aircraft::is_circling() const
{
    return !is_at_terminal && !has_terminal() && !landing_gear_deployed;
}

bool Aircraft::is_low_on_fuel() const
{
    return fuel < 200;
}

void Aircraft::refill(u_int& fuel_stock)
{
    u_int to_refill = std::min((int)fuel_stock, 3000 - fuel);
    fuel_stock -= to_refill;
    fuel += to_refill;
    std::cout << "Refilled " << flight_number << " with " << to_refill << " litters of fuel" << std::endl;
}