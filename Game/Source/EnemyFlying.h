#ifndef __ENEMY_FLYING_H__
#define __ENEMY_FLYING_H__

#include "Enemy.h"

class EnemyFlying : public Enemy
{
public:
	// Constructor (x y coordinates in the world)
	// Creates animation and movement data and the collider
	EnemyFlying(int x, int y, EnemyType typeOfEnemy);

	// The enemy is going to follow the different steps in the path
	// Position will be updated depending on the speed defined at each step
	void Update(float dt);

	EnemyType type;
private:
	// This enemy has one sprite and one frame
	// We are keeping it an animation for consistency with other enemies
	Animation flying;
	Animation hurt;
	Animation attack;
};

#endif // __ENEMY_GROUND_H__