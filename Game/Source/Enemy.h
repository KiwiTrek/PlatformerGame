#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "Animation.h"
#include "Physics.h"
#include "DynArray.h"

struct SDL_Texture;
class Collider;

enum EnemyType;

class Enemy : public Entity
{
public:
	// Constructor
	Enemy(int x, int y, EnemyType enemyType, Entity* playerPointer);

	// Destructor
	virtual ~Enemy();

	// Called from inhering enemies' Udpate
	virtual bool Update(float dt);

	// Called from ModuleEnemies' Update
	virtual bool Draw();

	// Collision response
	void OnCollision(Collider* c1, Collider* c2);

public:
	// Path related variables
	DynArray<iPoint> path;
	int pathCount = 0;

	EnemyType type = EnemyType::NO_TYPE;
protected:
	Entity* player;
	// General enemy size
	int enemySize;

	//Pathfinding related variables
	iPoint pastDest;
	int i;
	int counterTile;

	// State changes
	bool attackChange = false;
	bool hurtChange = false;
};

#endif // __ENEMY_H__