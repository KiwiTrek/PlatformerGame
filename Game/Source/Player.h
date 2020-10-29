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
	int playerSize = 128;
	Animation idle;	//9f
	Animation run;	//8f
	Animation jumpPrep;	//2f Prep; 4f Mid; 1f Land;	else frame 3;
	Animation jumpMid;
	Animation jumpLand;
	Animation death; //5f
	Animation wallJump;

	Animation* currentAnimation = &idle;

	int GetColliderId(int x, int y, bool isFruit = false, bool isObject = false) const;
	Collider* playerCollider = nullptr;

	iPoint spawnPoint;

	Physics playerPhysics;
	fPoint speed;

	unsigned int deadFx;

	bool godMode;
	bool keyPressed;
	bool isJumping;
	bool isDead;
	bool invert;
};

#endif // !__PLAYER_H__