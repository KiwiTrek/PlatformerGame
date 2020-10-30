#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "Point.h"

#include "SDL/include/SDL.h"

struct Physics {
public:
    void UpdatePhysics(int& _x, int& _y, float& _sx, float& _sy) {
        if (axisX) {
            _x = _x + _sx * deltaTime;
        }
        if (axisY) {
            //Euler
            //_y = _y + _sy * deltaTime;
            // Verlet
            _y = _y + _sy * deltaTime + (gravity * deltaTime * deltaTime * 0.5);


            _sy = _sy + gravity * deltaTime;
        }
    }

    bool axisX;
    bool axisY;
    float gravity = 950.0f;
    float deltaTime = 1.0f / 60.0f;
};

#endif // !__PHYSICS_H__