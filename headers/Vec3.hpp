#pragma once

namespace Math{
    template<typename T>
    struct Vec3{
        T x;
        T y;
        T z;

        Vec3(): x(0), y(0), z(0){}

        Vec3(T a, T b, T c): x(a), y(b), z(c){}

        ~Vec3(){}

        double length(){
            return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
        }

        Vec3<T> normalized(){
            double len = this->length();
            return Vec3<T>(x/len, y/len, z/len);
        }

        Vec3<T> operator*(Vec3<T> b){
            Vec3<T> res;
            res.x = y*b.z - z*b.y;
            res.y = z*b.x - x*b.z;
            res.z = x*b.y - y*b.x;
            return res;
        }

        Vec3<T> operator*(float c){
            Vec3<T> res;
            res.x = x*c;
            res.y = y*c;
            res.z = z*c;
            return res;
        }

        Vec3<T> operator-(Vec3<T> b){
            Vec3<T> res;
            res.x = x - b.x;
            res.y = y - b.y;
            res.z = z - b.z;
            return res;
        }

        Vec3<T> operator+(Vec3<T> b){
            Vec3<T> res;
            res.x = x + b.x;
            res.y = y + b.y;
            res.z = z + b.z;
            return res;
        }

        Vec3<T> operator+=(Vec3<T> b){
            x += b.x;
            y += b.y;
            z += b.z;
            return *this;
        }

        Vec3<T> operator-=(Vec3<T> b){
            x -= b.x;
            y -= b.y;
            z -= b.z;
            return *this;
        }

        T dot(Vec3<T> b){
            return x*b.x + y*b.y + z*b.z;
        }
    };
}
