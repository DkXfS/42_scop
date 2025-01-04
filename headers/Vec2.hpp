#pragma once

namespace Math{
    template<typename T>
    struct Vec2{
        T x;
        T y;

        Vec2(): x(0), y(0){}

        Vec2(T a, T b): x(a), y(b){}

        ~Vec2(){}

        double length(Vec2<T> v){
            return sqrt(pow(v.x, 2) + pow(v.y, 2));
        }

        Vec2<T> normalize(Vec2<T> v){
            double len = length(v);
            v.x /= len;
            v.y /= len;
            return v;
        }

        Vec2<T> operator-(Vec2<T> b){
            Vec2<T> res;
            res.x = this.x - b.x;
            res.y = this.y - b.y;
            return res;
        }

        T dot(Vec2<T> a, Vec2<T> b){
            return a.x*b.x + a.y*b.y;
        }
    };
}
