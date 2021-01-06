#ifndef __ENEMY_GROUND_H__
#define __ENEMY_GROUND_H__

#include "Enemy.h"

class EnemyGround : public Enemy
{
public:
	// Constructor
	EnemyGround(int x, int y, EnemyType typeOfEnemy, Entity* playerPointer);

	// Called each loop iteration
	bool Update(float dt);

	EnemyType type;
private:
	// Animation
	Animation idle;
	Animation walking;
	Animation hurt;
	Animation attack;
};

#endif // __ENEMY_GROUND_H__