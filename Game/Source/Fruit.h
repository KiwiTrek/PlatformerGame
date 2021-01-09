#ifndef __FRUIT_H__
#define __FRUIT_H__

#include "Entity.h"

struct Collider;

class Fruit : public Entity
{
public:
	// Constructor
	Fruit(int x, int y);

	// Blit
	bool Draw();

	// Collision response
	void OnCollision(Collider* c1, Collider* c2);

private:
	SDL_Rect fruitSect;
};

#endif // !__FRUIT_H__