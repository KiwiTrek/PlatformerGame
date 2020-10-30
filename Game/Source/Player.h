#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Module.h"
#include "Animation.h"
#include "Physics.h"

#include "SDL/include/SDL.h"

struct Collider;
class Player : public Module {
public:

	Player();

	void Init();

	// Destructor
	virtual ~Player();

	// Called before player is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&);

	SDL_Texture* playerTex;
	SDL_Rect playerRect = { 64 * 3,64 * 8,44,72 };

private:
	// Different collision types
	enum CollisionType {
		DOUBLE_AIR,
		DOUBLE_SOLID,
		SOLID_AIR,
		AIR_SOLID
	};

	int playerSize = 128;
	Animation idle;	//9f
	Animation run;	//8f
	Animation jumpPrep;	//2f Prep; 4f Mid; 1f Land;	else frame 3;
	Animation jumpMid;
	Animation jumpLand;
	Animation death; //5f
	Animation wallJump;

	Animation* currentAnimation = &idle;

	// Gets the value of a property in a given tile
	int GetTileProperty(int x, int y, const char* property, bool notMovCollision = false, bool isObject = false) const;
	// Gets the coordinates of the spawn point
	iPoint GetSpawnPoint();

	Collider* playerCollider = nullptr;
	bool positiveSpeedX = true;
	bool positiveSpeedY = true;
	// Returns the type of collision depending on two given tiles (movement collisions only)
	CollisionType GetCollisionType(int A, int B) const;

	iPoint spawnPoint;

	Physics playerPhysics;
	fPoint speed;
	int jumpCounter;
	/*
	jumpCounter should decrease when dropped from platform
	wallJump:	counts as double jump
		- if walljump, should decrease to 0
		- if touches another wall, is in air and jumpCounter = 0, should add 1 jump
	*/

	unsigned int deadFx;
	unsigned int jumpFx;
	unsigned int doubleJumpFx;
	unsigned int fruitFx;

	bool godMode;
	bool keyPressed;
	bool isJumping;
	bool isDead;
	bool invert;
	bool debugDraw;
	bool once;
};

#endif // !__PLAYER_H__