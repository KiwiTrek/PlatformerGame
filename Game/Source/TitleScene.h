#ifndef __TITLE_SCENE_H__
#define __TITLE_SCENE_H__

#include "Module.h"

struct SDL_Texture;

class TitleScene : public Module
{
public:

	TitleScene();

	// Destructor
	virtual ~TitleScene();

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
	SDL_Texture* titleScreen;
	SString folderTexture;
	SString folderAudioMusic;
};

#endif // __TITLE_SCENE_H__