#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	void Init();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

private:
	SDL_Color sky;
	SDL_Texture* clouds;
	SDL_Texture* mountainsBack;
	SDL_Texture* mountainsFront;
	SString folderTexture;
	SString folderAudioMusic;

	// Score Handling
	int font = -1;

	char score[8] = { "\0" };
};

#endif // __SCENE_H__