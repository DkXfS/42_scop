#pragma once

#include "Math.hpp"

namespace Math{
    template<typename T>
    struct Quaternion{
        T x, y, z, s;

        Quaternion(): x(0), y(0), z(0), s(0){}

        Quaternion(T x, T y, T z, T s): x(x), y(y), z(z), s(s){}

        Quaternion(Vec3<T> axis, T radians){
            axis = axis.normalized();
            radians /= 2;
            T sine = sin(radians);
            s = cos(radians);
            x = sine * axis.x;
            y = sine * axis.y;
            z = sine * axis.z;
        }

        Math::Mat4<T> getMatrix(){
            T arr[] = {1 - 2*y*y - 2*z*z,   2*x*y - 2*s*z,       2*x*z + 2*s*y,       0,
                       2*x*y + 2*s*z,       1 - 2*x*x - 2*z*z,   2*y*z - 2*s*x,       0,
                       2*x*z - 2*s*y,       2*y*z + 2*s*x,       1 - 2*x*x - 2*y*y,   0,
                       0,                   0,                   0,                   1,};
            return Mat4<T>(arr);
        }
    };
}
