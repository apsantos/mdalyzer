#ifndef MDALYZER_UTILS_VECTOR_MATH_H_
#define MDALYZER_UTILS_VECTOR_MATH_H_


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
	
    inline Vector3<T> operator- (const Vector3<T>& a) const
        {
	    return Vector3<T>(x-a.x,y-a.y,z-a.z);
        }
	
	inline Vector3<T>& operator+= (const Vector3<T>& a)
        {
	    x += a.x;
	    y += a.y;
	    z += a.z;
	    return *this;
        }
        
	inline Vector3<T>& operator-= (const Vector3<T>& a)
        {
	    x -= a.x;
	    y -= a.y;
	    z -= a.z;
	    return *this;
        }
    
    inline bool operator== (const Vector3<T>& a) const
        {
        return (x == a.x && y == a.y && z == a.z);
        }
        
    inline bool operator!= (const Vector3<T>& a) const
        {
        return (x != a.x || y != a.y || z != a.z);
        }
    
    inline double dot(const Vector3<T>& a) const
        {
        return x*a.x + y*a.y + z*a.z;
        }
        
    //! this.cross(a) = this x a
    inline Vector3<T> cross(const Vector3<T>& a) const
        {
        return Vector3<T>(y*a.z-z*a.y, z*a.x-x*a.z, x*a.y-y*a.x);
        }
    };

#endif //MDALYZER_UTILS_VECTOR_MATH_H_
