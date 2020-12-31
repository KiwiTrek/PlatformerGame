#ifndef __DEATH_SCENE_H__
#define __DEATH_SCENE_H__

#include "Module.h"

#include "GuiButton.h"

struct SDL_Texture;

class DeathScene : public Module
{
public:
	// Constructor
	DeathScene();

	// Destructor
	virtual ~DeathScene();

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

	bool OnGuiMouseClickEvent(GuiControl* control);

private:
	SDL_Texture* deathScreen;
	SString folderTexture;
	SString folderAudioMusic;

	int state;
	float alpha;
	float timer;

	GuiButton* btnTitle;
	float dtTmp;

	GuiButton* btnExit;
	bool exitRequest;
};

#endif // __DEATH_SCENE_H__