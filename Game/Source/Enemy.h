#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Point.h"
#include "Animation.h"

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
	iPoint position;

	// The enemy's texture
	SDL_Texture* texture = nullptr;
	SDL_Texture* deathTexture = nullptr;

	// The enemy's collider
	Collider* collider = nullptr;

	// Sound fx when destroyed
	int chasingFx = 0;
	int destroyedFx = 0;

protected:
	// A ptr to the current animation
	Animation* currentAnim = nullptr;
	int enemySize = 64;

	EnemyType type = EnemyType::NO_TYPE;

	// Original spawn position. Stored for movement calculations
	iPoint spawnPos;
};

#endif // __ENEMY_H__