#pragma once

#include "math.hpp"

struct Camera{
    Math::Vec3<float> position;
    float fov;
    float movSpeed;

    enum movDirections{
        Fore,
        Back,
        Right,
        Left,
        Up,
        Down,
    };

    Camera(): frontVec(0,0,-1), rightVec(1, 0, 0), upVec(0, 1, 0), worldUpVec(0, 1, 0), movSpeed(10), fov(Math::radians(50)), tracking(false){}

    ~Camera(){}

    void recalculateVectors(){
        frontVec = (trackedPoint - position).normalized();
        rightVec = (frontVec * worldUpVec).normalized();
        upVec = (rightVec * frontVec).normalized();
    }

    void track(Math::Vec3<float> p){
        trackedPoint = p;
        recalculateVectors();
        tracking = true;
    }

    void stopTracking(){
        tracking = false;
    }

    void move(movDirections direction, float time){
        float maxdT = 0.0001;

        // Integrate the position calculation to avoid quickly reaching visible errors
        while(time > 0){
            if(time < maxdT)
                maxdT = time;

            if(direction == Fore)
                position += frontVec * movSpeed * maxdT;
            else if(direction == Back)
                position -= frontVec * movSpeed * maxdT;
            else if(direction == Left)
                position -= rightVec * movSpeed * maxdT;
            else if(direction == Right)
                position += rightVec * movSpeed * maxdT;
            else if(direction == Up)
                position += upVec * movSpeed * maxdT;
            else if(direction == Down)
                position -= upVec * movSpeed * maxdT;

            if(tracking)
                recalculateVectors();
            time -= maxdT;
        }
    }

    Math::Mat4<float> getViewMatrix(){
        if(tracking)
            return Math::viewMatrix(position, trackedPoint, Math::Vec3(0.0f, 1.0f, 0.0f));
        else
            return Math::viewMatrix(position, position + frontVec, Math::Vec3(0.0f, 1.0f, 0.0f));
    }

    private:
        bool tracking;
        Math::Vec3<float> trackedPoint;
        Math::Vec3<float> frontVec;
        Math::Vec3<float> rightVec;
        Math::Vec3<float> upVec;
        Math::Vec3<float> worldUpVec;
};
