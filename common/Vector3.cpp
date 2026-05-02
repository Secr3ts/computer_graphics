#include "Vector3.h"
#include <cmath>

template <typename T>
void Vector3<T>::normalize()
{
	T len = static_cast<T>(std::sqrt(static_cast<double>(mX * mX + mY * mY + mZ * mZ)));

	if (len != 0)
	{
		mX /= len;
		mY /= len;
		mZ /= len;
	}
}

template <typename T>
T Vector3<T>::length()
{
	return static_cast<T>(std::sqrt(static_cast<double>(mX * mX + mY * mY + mZ * mZ)));
}

// Explicit instantiation for float to provide symbols for linker
template class Vector3<float>;
