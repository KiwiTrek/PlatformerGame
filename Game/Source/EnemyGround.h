#ifndef __ENEMY_GROUND_H__
#define __ENEMY_GROUND_H__

#include "Enemy.h"

class Collider;
class AudioManager;
class EntityManager;
class PathFinding;
class Map;
class Enemy;
class Entity;
enum EnemyType;

class EnemyGround : public Enemy
{
public:
	// Constructor
	EnemyGround(int x, int y, EntityManager* listener, EnemyType typeOfEnemy);

	// Called each loop iteration
	bool Update(float dt, Map* map, AudioManager* audio, PathFinding* pathfinding, Entity* player);

	EnemyType type;
private:
	Animation idle;
	Animation walking;
	Animation hurt;
	Animation attack;
};

#endif // __ENEMY_GROUND_H__