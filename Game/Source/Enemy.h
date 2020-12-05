#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Point.h"
#include "Animation.h"
#include "Physics.h"
#include "DynArray.h"

struct SDL_Texture;
class Collider;

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
	// Saves the spawn position for later movement calculations
	Enemy(int x, int y, EnemyType type);

	// Destructor
	virtual ~Enemy();

	// Called from inhering enemies' Udpate
	// Updates animation and collider position
	virtual void Update(float dt);

	// Called from ModuleEnemies' Update
	virtual void Draw();

	// Collision response
	// Triggers an animation and a sound fx
	
	void OnCollision(Collider* c1, Collider* c2);

public:
	// The current position in the world
	SDL_Rect enemyRect;

	// The enemy's texture
	SDL_Texture* texture = nullptr;

	// Add to despawn queue
	bool pendingToDelete = false;

	int pathCount = 0;

	// The enemy's collider
	Collider* collider = nullptr;

	EnemyType type = EnemyType::NO_TYPE;

	// The enemy's path
	DynArray<iPoint> path;

	// The enemy's physics
	Physics enemyPhysics;
	iPoint nextFrame;

	// Sound fx when destroyed
	int chasingFx = 0;
	int destroyedFx = 0;

protected:
	// A ptr to the current animation
	Animation* currentAnim = nullptr;
	bool invert = false;
	int enemySize = 64;

	// Original spawn position. Stored for movement calculations
	iPoint spawnPos;

	iPoint pastDest;
	int i;

	//DynArray<iPoint> currentPath;

	// State changes
	bool attackChange = false;
	bool hurtChange = false;
};

#endif // __ENEMY_H__