#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "Point.h"

#include "SDL/include/SDL.h"

struct Physics {
public:
    void UpdatePhysics(int& _x, int& _y, float& _sx, float& _sy) {
        if (axisX) { _x = _x + _sx * deltaTime; }
        if (axisY) {
            _y = _y + _sy * deltaTime + gravity * deltaTime * deltaTime / 2;
            _sy = _sy + gravity * deltaTime;
        }
    }
    bool CheckMovementCollision(SDL_Rect A, SDL_Rect B) {
        //The sides of the rectangles
        int leftA, leftB;
        int rightA, rightB;
        int topA, topB;
        int bottomA, bottomB;

        //Calculate the sides of rect A
        leftA = A.x;
        rightA = A.x + A.w;
        topA = A.y;
        bottomA = A.y + A.h;

        //Calculate the sides of rect B
        leftB = B.x;
        rightB = B.x + B.w;
        topB = B.y;
        bottomB = B.y + B.h;
        //If any of the sides from A are outside of B
        if (bottomA <= topB) { return false; }

        if (topA >= bottomB) { return false; }

        if (rightA <= leftB) { return false; }

        if (leftA >= rightB) { return false; }

        //If none of the sides from A are outside B
        return true;
    }

    bool axisX;
    bool axisY;
    float gravity = 600.0f;
    float deltaTime = 1.0f / 60.0f;
};

#endif // !__PHYSICS_H__