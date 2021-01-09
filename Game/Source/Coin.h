#ifndef __COIN_H__
#define __COIN_H__

#include "Entity.h"

struct Collider;

class Coin : public Entity
{
public:
	// Constructor
	Coin(int x, int y);

	// Called each loop iteration
	bool Update(float dt);

	// Blit
	bool Draw();

	// Collision response
	void OnCollision(Collider* c1, Collider* c2);

private:
	// Animation
	Animation rotating;
};

#endif // !__COIN_H__