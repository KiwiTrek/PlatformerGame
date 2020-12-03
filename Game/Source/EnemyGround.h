#ifndef __ENEMY_GROUND_H__
#define __ENEMY_GROUND_H__

#include "Enemy.h"

class EnemyGround : public Enemy
{
public:
	// Constructor (x y coordinates in the world)
	// Creates animation and movement data and the collider
	EnemyGround(int x, int y, EnemyType typeOfEnemy);

	// The enemy is going to follow the different steps in the path
	// Position will be updated depending on the speed defined at each step
	void Update(float dt);

	EnemyType type;
private:
	// This enemy has one sprite and one frame
	// We are keeping it an animation for consistency with other enemies
	Animation idle;
	Animation walking;
	Animation hurt;
	Animation attack;
	bool onceAnim;
};

#endif // __ENEMY_GROUND_H__