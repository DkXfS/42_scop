#pragma once

namespace Math{
    template<typename T>
    struct Vec4{
        T x;
        T y;
        T z;
        T w;

        Vec4(): x(0), y(0), z(0), w(0){}

        Vec4(T a, T b, T c, T d): x(a), y(b), z(c), w(d){}

        ~Vec4(){}

        T dot(Vec4<T> b){
            return x*b.x + y*b.y + z*b.z + w*b.w;
        }
    };
}
