#ifndef __ENEMY_FLYING_H__
#define __ENEMY_FLYING_H__

#include "Enemy.h"

class EnemyFlying : public Enemy
{
public:
	// Constructor
	EnemyFlying(int x, int y, EnemyType typeOfEnemy, Entity* playerPointer);

	// Called each loop iteration
	bool Update(float dt);

	EnemyType type;
private:
	Animation flying;
	Animation hurt;
	Animation attack;
};

#endif // __ENEMY_GROUND_H__