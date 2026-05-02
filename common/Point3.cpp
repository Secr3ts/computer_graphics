#include "Point3.h"

template <typename T>
bool Point3<T>::pos() { // STUB TO AVOID G++ COMPLAINING THAT THERE IS NO FILE
    if (x > 0 && y > 0 && z > 0) {
        return true;
    }
    return false;
}