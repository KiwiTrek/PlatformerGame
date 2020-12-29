#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Point.h"
#include "Animation.h"
#include "Physics.h"
#include "DynArray.h"

struct SDL_Texture;
class Collider;

// Enemy Type enum
enum EnemyType
{
	NO_TYPE,
	GROUND,
	FLYING
};

class Enemy
{
public:
	// Constructor
	Enemy(int x, int y, EnemyType type);

	// Destructor
	virtual ~Enemy();

	// Called from inhering enemies' Udpate
	virtual void Update(float dt);

	// Called from ModuleEnemies' Update
	virtual void Draw();

	// Collision response
	void OnCollision(Collider* c1, Collider* c2);

public:
	// The current position in the world
	SDL_Rect enemyRect;

	// Enemy physics related variables
	Physics enemyPhysics;
	iPoint nextFrame;

	// Path related variables
	DynArray<iPoint> path;
	int pathCount = 0;

	// Add to despawn queue
	bool pendingToDelete = false;

	SDL_Texture* texture = nullptr;
	EnemyType type = EnemyType::NO_TYPE;
	int destroyedFx = 0;
	Collider* collider = nullptr;

protected:
	// General enemy size
	int enemySize;

	// Original spawn position
	iPoint spawnPos;

	//Pathfinding related variables
	iPoint pastDest;
	int i;
	int counterTile;

	// State changes
	bool attackChange = false;
	bool hurtChange = false;

	// Animation related variables
	Animation* currentAnim = nullptr;
	bool invert = false;
};

#endif // __ENEMY_H__