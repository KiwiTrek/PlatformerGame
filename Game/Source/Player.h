#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Module.h"
#include "Animation.h"
#include "Physics.h"

#include "SDL/include/SDL.h"

struct Collider;
class Player : public Module
{
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

	void OnCollision(Collider* c1, Collider* c2);
	void ResolveCollisions(iPoint nextFrame, bool goingDown);

	SDL_Texture* playerTex;
	SDL_Texture* playerHeart;
	SDL_Rect playerRect;
	iPoint spawnPoint;

private:

	// Different collision types
	enum CollisionType
	{
		DOUBLE_AIR,
		DOUBLE_SOLID,
		SOLID_AIR,
		AIR_SOLID
	};

	int playerSize;
	Animation idle;
	Animation run;
	Animation jumpPrep;
	Animation jumpMid;
	Animation jumpLand;
	Animation attack;
	Animation hit;
	Animation death;
	Animation wallJump;

	Animation* currentAnimation = &idle;

	// Gets the coordinates of the spawn point
	iPoint GetSpawnPoint();

	Collider* playerCollider = nullptr;
	Collider* hurtBox = nullptr;

	bool changeSpawn;
	iPoint prevPoint;

	Physics playerPhysics;
	iPoint nextFrame;
	int jumpCounter;

	unsigned int deadFx;
	unsigned int jumpFx;
	unsigned int doubleJumpFx;
	unsigned int fruitFx;
	unsigned int hitFx;
	unsigned int slashFx;
	unsigned int checkpointFx;

	bool isDead = false;
	int hitCD = 5;
	int lives = 3;

	bool godMode = false;
	bool keyPressed = false;
	bool isJumping = false;
	bool isHit = false;
	bool isAttacking = false;
	bool invert = false;
	bool debugDraw = false;
	bool once = true;
	bool onceAnim = true;

	SString folderTexture;
	SString folderAudioFx;
};

#endif // !__PLAYER_H__