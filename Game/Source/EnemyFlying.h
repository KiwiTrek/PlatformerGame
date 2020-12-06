#ifndef __ENEMY_FLYING_H__
#define __ENEMY_FLYING_H__

#include "Enemy.h"

class EnemyFlying : public Enemy
{
public:
	// Constructor
	EnemyFlying(int x, int y, EnemyType typeOfEnemy);

	// Called each loop iteration
	void Update(float dt);

	EnemyType type;
private:
	Animation flying;
	Animation hurt;
	Animation attack;
};

#endif // __ENEMY_GROUND_H__