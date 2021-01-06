#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"

class Entity;
struct SDL_Texture;

class Scene : public Module
{
public:
	// Constructor
	Scene();

	// Called when program is executed
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

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Load/Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&);

	// Loading variables
	int scoreValue = 0;
	int coinCounter = 0;
	int fruitCounter = 0;

private:
	Entity* player = nullptr;
	iPoint cameraPos;
	iPoint cameraSize;

	// Background
	SDL_Color sky;
	SDL_Texture* clouds;
	SDL_Texture* mountainsBack;
	SDL_Texture* mountainsFront;

	SString folderTexture;
	SString folderAudioMusic;

	// GUI
	int font = -1;

	// Timer handling
	float timerValue;
	SString timerTitle;
	int offsetTimer;
	char timer[6] = { "\0" };

	// Score handling
	SString scoreTitle;
	int offsetScore;
	char score[6] = { "\0" };

	// Fruit & Coin handling
	SDL_Texture* ui;
	SDL_Rect uiFruit;
	char scoreFruit[4] = { "\0" };
	SDL_Rect uiCoin;
	char scoreCoin[4] = { "\0" };
};

#endif // __SCENE_H__