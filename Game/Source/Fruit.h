#ifndef __FRUIT_H__
#define __FRUIT_H__

#include "Entity.h"

#include "SDL.h"

struct Collider;
class Fruit : public Entity
{
public:
	// Constructor
	Fruit(int x, int y);

	bool Draw();

	// Collision response
	void OnCollision(Collider* c1, Collider* c2);

private:
	SDL_Rect fruitSect;

	uint fruitFx;
};

#endif // !__FRUIT_H__