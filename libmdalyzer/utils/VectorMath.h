#ifndef __VECTOR_MATH_H__
#define __VECTOR_MATH_H__


template <typename T>
struct Vector3
    {
    T x,y,z;
    Vector3() : x(0), y(0), z(0) {}
    Vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
    
    inline Vector3<T> operator+ (const Vector3<T>& a) const
        {
	    return Vector3<T>(x+a.x,y+a.y,z+a.z);
        }
	
	inline Vector3<T>& operator+=(const Vector3<T>& a)
        {
	    x += a.x;
	    y += a.y;
	    z += a.z;
	    return *this;
        }
    };

#endif //__VECTOR_MATH_H__
