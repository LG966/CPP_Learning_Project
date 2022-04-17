#include "tower_sim.hpp"

void test_generic_points()
{
    Point2D p1;
    Point2D p2;
    auto p3 = p1 + p2;
    p1 += p2 + p3;
    p1 *= 3; // ou 3.f, ou 3.0 en fonction du type de Point
}

int main(int argc, char** argv)
{
    test_generic_points();

    TowerSimulation simulation { argc, argv };
    simulation.launch();

    return 0;
}