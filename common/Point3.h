#pragma once
#ifndef POINT3_H
#define POINT3_H

#include <cmath>
#include <iostream>

template <typename T>
struct Point3
{
    Point3(): x(0), y(0), z(0) {};
    Point3(T mX, T mY, T mZ) : x(mX), y(mY), z(mZ) {};

    T x;
    T y;
    T z;

    bool pos();
};

#endif