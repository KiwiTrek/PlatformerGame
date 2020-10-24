#ifndef __MODULE_PLAYER_H__
#define __MODULE_PLAYER_H__

#include "Module.h"

#include "SDL/include/SDL.h"

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
	SDL_Rect playerRect;
	SDL_Rect playerIdle;
private:
};

#endif // !__MODULE_PLAYER_H__

