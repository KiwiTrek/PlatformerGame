#ifndef __LOGO_SCENE_H__
#define __LOGO_SCENE_H__

#include "Module.h"

struct SDL_Texture;

class LogoScene : public Module
{
public:
	// Constructor
	LogoScene();

	// Destructor
	virtual ~LogoScene();

	// Called when program is executed
	void Init();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

private:
	SDL_Texture* logo;
	uint logoFx;
	SString folderTexture;
	SString folderAudioFx;
	bool onceTimer = true;
	SDL_Rect window;

	float timer = 0.0f;
	int state = 0;
	float logoAlpha = 0.0f;
};

#endif // __LOGO_SCENE_H__