#pragma once

#include <math.h>
#include "Mat4.hpp"
#include "Vec3.hpp"
#include "Vec2.hpp"
#include "Vec4.hpp"
#include "Quaternion.hpp"

namespace Math{
    float radians(float degrees){ return degrees * M_PI/180; }

    float degrees(float radians){ return radians * 180/M_PI; }

    template<typename T>
    Mat4<T> perspectiveMatrix(T fov, T aspect, T nearPlane, T farPlane){
        T halfFovTan = tan(fov/2);
        Mat4<T> perspProj = Mat4<T>();

        perspProj[0][0] = 1/(aspect * halfFovTan);
        perspProj[1][1] = 1/halfFovTan;
        perspProj[2][2] = -(farPlane + nearPlane)/(farPlane - nearPlane);
        perspProj[2][3] = -1;
        perspProj[3][2] = -(2 * farPlane * nearPlane)/(farPlane - nearPlane);

        return perspProj;
    }

    template<typename T>
    Mat4<T> viewMatrix(Vec3<T> position, Vec3<T> lookAt, Vec3<T> worldUp){
        Vec3<T> camForward = (lookAt - position).normalized();
        Vec3<T> camRight = (camForward * worldUp).normalized();
        Vec3<T> camUp = camRight * camForward;

        T arr[] = {
            camRight.x,     camRight.y,    camRight.z,  - position.dot(camRight),
            camUp.x,        camUp.y,       camUp.z,     - position.dot(camUp),
            -camForward.x, -camForward.y, -camForward.z, position.dot(camForward),
            0,              0,             0,            1
            };
        return Mat4<T>(arr);
    }

    template<typename T>
    Mat4<T> translationMatrix(Mat4<T> matrix, Vec3<T> translate){
        Vec4<T> a(matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]);
        Vec4<T> b(matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3]);
        Vec4<T> c(matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]);
        Vec4<T> d(matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
        Vec4<T> tr(translate.x, translate.y, translate.z, 1);
        matrix[3][0] = a.dot(tr);
        matrix[3][1] = b.dot(tr);
        matrix[3][2] = c.dot(tr);
        matrix[3][3] = d.dot(tr);
        return matrix;
    }
}
