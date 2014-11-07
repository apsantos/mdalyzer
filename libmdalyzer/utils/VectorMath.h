#ifndef __VECTOR_MATH_H__
#define __VECTOR_MATH_H__


template <typename T>
struct Vector3
    {
    T x,y,z;
    Vector3() : x(0), y(0), z(0) {}
    Vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
    };

#endif //__VECTOR_MATH_H__
