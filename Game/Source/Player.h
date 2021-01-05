#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Animation.h"
#include "Physics.h"

#include "SDL.h"

struct Collider;
class Player : public Entity
{
public:
	// Constructor
	Player(int x, int y);

	// Called each loop iteration
	bool Update(float dt);

	// Called after all Updates
	//bool PostUpdate(float dt);

	bool Draw();

	// Collision response
	void OnCollision(Collider* c1, Collider* c2);

	// Loading variables
	int lives = 3;
	float resultingMove = 0.0f;
	bool firstCheckpoint = true;

private:
	// Gets the coordinates of the spawn point
	iPoint GetSpawnPoint();
	Collider* hurtBox = nullptr;

	int jumpCounter;
	int hitCD = 5;
	int playerSize;

	bool keyPressed = false;
	bool godMode = false;
	bool isHit = false;
	bool isJumping = false;
	bool isAttacking = false;
	bool invert = false;
	bool debugDraw = false;
	bool once = true;
	bool onceCheckpoint = true;
	bool updateCamera = false;

	Animation idle;
	Animation run;
	Animation jumpPrep;
	Animation jumpMid;
	Animation jumpLand;
	Animation attack;
	Animation hit;
	Animation death;
	Animation wallJump;

	bool heartLess;
	Animation heartDestroyed; //19 f
	bool heartMore;
	Animation heartRecovered; //11 f

	uint jumpFx;
	uint doubleJumpFx;
	uint fruitFx;
	uint hitFx;
	uint slashFx;
	uint checkpointFx;
};

#endif // !__PLAYER_H__