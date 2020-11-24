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

	SDL_Texture* playerTex;
	SDL_Rect playerRect;

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
	Animation death;
	Animation wallJump;

	Animation* currentAnimation = &idle;

	// Gets the value of a property in a given tile
	int GetTileProperty(int x, int y, const char* property, bool notMovCollision = false, bool isObject = false) const;
	// Gets the coordinates of the spawn point
	iPoint GetSpawnPoint();

	Collider* playerCollider = nullptr;
	bool positiveSpeedX;
	bool positiveSpeedY;
	// Returns the type of collision depending on two given tiles (movement collisions only)
	CollisionType GetCollisionType(int A, int B) const;

	iPoint spawnPoint;

	Physics playerPhysics;
	fPoint speed;
	int jumpCounter;

	unsigned int deadFx;
	unsigned int jumpFx;
	unsigned int doubleJumpFx;
	unsigned int fruitFx;

	bool godMode = false;
	bool isDead = false;
	bool keyPressed = false;
	bool isJumping = false;
	bool invert = false;
	bool debugDraw = false;
	bool once = true;
	bool onceAnim = true;

	SString folderTexture;
	SString folderAudioFx;
};

#endif // !__PLAYER_H__