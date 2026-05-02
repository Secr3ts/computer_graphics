#pragma once
#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>
#include <iostream>

#include "Point3.h"

template <typename T>
struct Vector3
{
	Vector3() : mX(0), mY(0), mZ(0) {};
	Vector3(T mX, T mY, T mZ) : mX(mX), mY(mY), mZ(mZ) {};
	
	T mX;
	T mY;
	T mZ;

	inline Vector3 operator+(const Vector3& rhs) const //Vector addition
	{
		return { mX + rhs.mX, mY + rhs.mY, mZ + rhs.mZ };
	}

	inline void operator+=(const Vector3& rhs) {
		mX += rhs.mX;
		mY += rhs.mY;
		mZ += rhs.mZ;
	}

	inline Vector3 operator-(const Vector3& rhs) const // Vector subtraction
	{
		return { mX - rhs.mX, mY - rhs.mY, mZ - rhs.mZ };
	}

	inline Vector3 operator-() const
	{
		return {
			-mX, -mY, -mZ
		};
	}

	inline Vector3 operator*(float scalar) const // Vector * scalar
	{
		return { mX * scalar, mY * scalar, mZ * scalar };
	}

	friend inline Vector3 operator*(float scalar, const Vector3& rhs) // Scalar * Vector
	{
		return { scalar * rhs.mX, scalar * rhs.mY, scalar * rhs.mZ };
	}
	inline float operator*(Vector3& rhs) // Dot product
	{
		float dotP = mX * rhs.mX + mY * rhs.mY + mZ * rhs.mZ;
		
		return dotP;
	}

	const inline Vector3 cross(const Vector3& rhs) // Vector * Vector
	{
		return { mY * rhs.mZ - mZ * rhs.mY, mZ * rhs.mX - mX * rhs.mZ, mX * rhs.mY - mY * rhs.mX };
	}

	inline bool operator==(const Vector3& rhs) const // Equality
	{
		const float EPS = 1e-6f;

		return std::fabs(mX - rhs.mX) < EPS && std::fabs(mY - rhs.mY) < EPS && std::fabs(mZ - rhs.mZ) < EPS;
	}
	inline bool operator!=(const Vector3& rhs) const // Not equal
	{
		const float EPS = 1e-6f;

		return std::fabs(mX - rhs.mX) > EPS && std::fabs(mY - rhs.mY) > EPS && std::fabs(mZ - rhs.mZ) > EPS;
	}

	friend inline std::ostream& operator<<(std::ostream& os, const Vector3& rhs)
	{
		os << "(" << rhs.mX << ", " << rhs.mY << ", " << rhs.mZ << ")";
		return os;
	}

	void normalize();
	T length();
};

#endif