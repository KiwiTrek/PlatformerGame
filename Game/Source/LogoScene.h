#ifndef __LOGO_SCENE_H__
#define __LOGO_SCENE_H__

#include "Module.h"

struct SDL_Texture;

class LogoScene : public Module
{
public:

	LogoScene();

	// Destructor
	virtual ~LogoScene();

	void Init();

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

	SDL_Texture* logo;
	int timer = 0;
	unsigned int logoFx;
	SString folderTexture;
	SString folderAudioFx;
	bool once = true;
};

#endif // __LOGO_SCENE_H__