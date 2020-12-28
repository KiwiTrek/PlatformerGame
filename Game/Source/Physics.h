#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "Point.h"

#include "SDL.h"

class Collider;
class Map;

struct Physics
{
public:
	void UpdatePhysics(iPoint& pos, float dt);

	// To know what direction the velocity is going
	void CheckDirection();

	// Collisions
	void ResolveCollisions(Map* map, SDL_Rect& currentFrame, iPoint nextFrame, bool goingLeft);

public:
	bool axisX;
	bool axisY;
	bool positiveSpeedY;
	bool verlet = true;
	fPoint speed;
	float gravity = 600.0f;
};

#endif // !__PHYSICS_H__