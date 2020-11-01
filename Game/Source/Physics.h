#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "Point.h"

#include "SDL/include/SDL.h"

struct Physics
{
public:
	void UpdatePhysics(int& x, int& y, float& sx, float& sy)
	{
		if (axisX)
		{
			x = x + sx * deltaTime;
		}
		if (axisY)
		{
			//Euler
			//y = y + sy * deltaTime;

			// Verlet
			y = y + sy * deltaTime + (gravity * deltaTime * deltaTime * 0.5);


			sy = sy + gravity * deltaTime;
		}
	}

	bool axisX;
	bool axisY;
	float gravity = 950.0f;
	float deltaTime = 1.0f / 60.0f;
};

#endif // !__PHYSICS_H__