#include "tower_sim.hpp"

#include "GL/opengl_interface.hpp"
#include "aircraft.hpp"
#include "airport.hpp"
#include "config.hpp"
#include "img/image.hpp"
#include "img/media_path.hpp"

#include <cassert>
#include <cstdlib>
#include <ctime>

using namespace std::string_literals;

TowerSimulation::TowerSimulation(int argc, char** argv) :
    help { (argc > 1) && (std::string { argv[1] } == "--help"s || std::string { argv[1] } == "-h"s) }
{
    MediaPath::initialize(argv[0]);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    GL::init_gl(argc, argv, "Airport Tower Simulation");

    create_keystrokes();

    init_airport();
    GL::move_queue.emplace(&aircraft_manager);
    init_aircraft_manager();
}

TowerSimulation::~TowerSimulation()
{
    delete airport;
}

void TowerSimulation::create_keystrokes()
{
    GL::keystrokes.emplace('x', []() { GL::exit_loop(); });
    GL::keystrokes.emplace('q', []() { GL::exit_loop(); });
    GL::keystrokes.emplace('c', [this]() { aircraft_factory->create_random_aircraft(); });
    GL::keystrokes.emplace('+', []() { GL::change_zoom(0.95f); });
    GL::keystrokes.emplace('-', []() { GL::change_zoom(1.05f); });
    GL::keystrokes.emplace('f', []() { GL::toggle_fullscreen(); });
    GL::keystrokes.emplace('i', []() { GL::increase_ticks(1); });
    GL::keystrokes.emplace('o', []() { GL::increase_ticks(-1); });
    GL::keystrokes.emplace('p', []() { GL::pause_now(); });
    GL::keystrokes.emplace(
        'r',
        [this]() { std::cout << "Crash count - " << this->aircraft_manager.get_crash_count() << std::endl; });

    for (size_t i = 0; i < 7; i++)
    {
        GL::keystrokes.emplace('0' + i,
                               [this, i]()
                               {
                                   auto airline = aircraft_factory->get_nth_airline(i);
                                   if (!airline.has_value())
                                   {
                                       std::cout << "This airline has not been assigned" << std::endl;
                                   }
                                   else
                                   {
                                       std::cout
                                           << "airline [" + airline.value() + "] has "
                                           << aircraft_manager.count_aircraft_with_airline(airline.value())
                                           << " aircrafts" << std::endl;
                                   }
                               });
    }
}

void TowerSimulation::display_help() const
{
    std::cout << "This is an airport tower simulator" << std::endl
              << "the following keysstrokes have meaning:" << std::endl;

    for (const auto& [key, _] : GL::keystrokes)
    {
        std::cout << key << ' ';
    }

    std::cout << std::endl;
}

void TowerSimulation::init_airport()
{
    // make sure the function is called only one with this trick
    static bool is_init = false;
    assert(!is_init);
    airport =
        new Airport { one_lane_airport, Point3D { 0.f, 0.f, 0.f },
                      new img::Image { one_lane_airport_sprite_path.get_full_path() }, aircraft_manager };

    // GL::display_queue.emplace_back(airport);
    GL::move_queue.emplace(airport);
    is_init = true;
}

void TowerSimulation::init_aircraft_manager()
{
    static bool is_init = false;
    assert(!is_init);
    std::vector<std::string> airlines { "AF", "LH", "EY", "DL", "KL", "BA", "AY", "EY" };

    aircraft_factory = new AircraftFactory(aircraft_manager, airlines, airport);
    aircraft_factory->create_aircraft_type(.02f, .05f, .02f, MediaPath { "l1011_48px.png" });
    aircraft_factory->create_aircraft_type(.02f, .05f, .02f, MediaPath { "b707_jat.png" });
    aircraft_factory->create_aircraft_type(.02f, .15f, .06f, MediaPath { "concorde_af.png" });
    is_init = true;
}

void TowerSimulation::launch()
{
    if (help)
    {
        display_help();
        return;
    }
    GL::loop();
}
