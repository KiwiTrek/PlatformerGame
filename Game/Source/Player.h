#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Module.h"
#include "Animation.h"
#include "Physics.h"

#include "SDL.h"

struct Collider;
class Player : public Module
{
public:
	// Constructor
	Player();

	// Called when program is executed
	void Init();

	// Destructor
	virtual ~Player();

	// Called before player is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Save/Load
	bool Save(pugi::xml_node&);
	bool Load(pugi::xml_node&);

	// Collision response
	void OnCollision(Collider* c1, Collider* c2);

	SDL_Texture* playerTex;
	SDL_Texture* playerHeart;
	SDL_Rect playerRect;
	iPoint spawnPoint;
	int score = 0;

private:
	// Gets the coordinates of the spawn point
	iPoint GetSpawnPoint();

	Animation* currentAnimation = &idle;

	Collider* playerCollider = nullptr;
	Collider* hurtBox = nullptr;

	Physics playerPhysics;
	iPoint nextFrame;
	int jumpCounter;
	int hitCD = 5;
	int lives = 3;
	int playerSize;

	bool keyPressed = false;
	bool godMode = false;
	bool isDead = false;
	bool isJumping = false;
	bool isHit = false;
	bool isAttacking = false;
	bool invert = false;
	bool debugDraw = false;
	bool once = true;
	bool onceCheckpoint = true;

	Animation idle;
	Animation run;
	Animation jumpPrep;
	Animation jumpMid;
	Animation jumpLand;
	Animation attack;
	Animation hit;
	Animation death;
	Animation wallJump;

	uint deadFx;
	uint jumpFx;
	uint doubleJumpFx;
	uint fruitFx;
	uint hitFx;
	uint slashFx;
	uint checkpointFx;

	SString folderTexture;
	SString folderAudioFx;
};

#endif // !__PLAYER_H__