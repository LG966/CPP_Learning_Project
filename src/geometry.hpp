#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>

template <size_t dimension, typename t> class Point
{
public:
    Point() {}
    std::array<t, dimension> values;

    template <typename... U> Point(t first, U&&... args) : values { first, std::forward<U>(args)... }
    {
        static_assert(sizeof...(args) + 1 == dimension, "Provided args length does not match dimension");
    }

    t& x() { return values[0]; }
    t x() const { return values[0]; }

    t& y()
    {
        static_assert(dimension >= 2);
        return values[1];
    }
    t y() const
    {
        static_assert(dimension >= 2);
        return values[1];
    }
    t z() const
    {
        static_assert(dimension >= 3);
        return values[2];
    }
    t& z()
    {
        static_assert(dimension >= 3);
        return values[2];
    }

    Point& operator+=(const Point& other)
    {
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(), std::plus<t>());
        return *this;
    }

    Point& operator-=(const Point& other)
    {
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(), std::minus<t>());
        return *this;
    }

    Point& operator*=(const Point& other)
    {
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(),
                       std::multiplies<t>());
        return *this;
    }

    Point& operator*=(const float scalar)
    {
        std::transform(values.begin(), values.end(), values.begin(),
                       [scalar](float coor) -> float { return coor * scalar; });
        return *this;
    }

    Point operator+(const Point& other) const
    {
        Point result = *this;
        result += other;
        return result;
    }

    Point operator-(const Point& other) const
    {
        Point result = *this;
        result -= other;
        return result;
    }

    Point operator*(const float scalar) const
    {
        Point result = *this;
        result *= scalar;
        return result;
    }

    Point operator*(const Point& other) const
    {
        Point result = *this;
        result *= other;
        return result;
    }

    Point operator-() const
    {
        Point rtr = *this;
        rtr *= (-1);
        return rtr;
    }

    float length() const
    {
        return std::sqrt(std::accumulate(values.begin(), values.end(), 0.0f,
                                         [](float acc, float coor) { return acc + coor * coor; }));
    }

    float distance_to(const Point& other) const { return (*this - other).length(); }

    Point& normalize(const float target_len = 1.0f)
    {
        const float current_len = length();
        if (current_len == 0)
        {
            throw std::logic_error("cannot normalize vector of length 0");
        }

        *this *= (target_len / current_len);
        return *this;
    }

    Point& cap_length(const float max_len)
    {
        assert(max_len > 0);

        const float current_len = length();
        if (current_len > max_len)
        {
            *this *= (max_len / current_len);
        }

        return *this;
    }
};

using Point3D = Point<3, float>;
using Point2D = Point<2, float>;

// our 3D-coordinate system will be tied to the airport: the runway is parallel to the x-axis, the z-axis
// points towards the sky, and y is perpendicular to both thus,
// {1,0,0} --> {.5,.5}   {0,1,0} --> {-.5,.5}   {0,0,1} --> {0,1}
inline Point2D project_2D(const Point3D& p)
{
    return { .5f * p.x() - .5f * p.y(), .5f * p.x() + .5f * p.y() + p.z() };
}
