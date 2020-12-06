#ifndef __APP_H__
#define __APP_H__

#include "Module.h"
#include "PerfTimer.h"
#include "Timer.h"

#include "List.h"

#include "PugiXml/src/pugixml.hpp"

#define CONFIG_FILENAME		"config.xml"
#define SAVE_STATE_FILENAME "save_game.xml"

// Modules
class Window;
class Input;
class Render;
class Textures;
class Fonts;
class Audio;
class Player;
class EnemyManagement;
class Map;
class Transition;
class Collisions;
class PathFinding;

class Scene;
class LogoScene;
class TitleScene;
class DeathScene;

class App
{
public:

	// Constructor
	App(int argc, char* args[]);

	// Destructor
	virtual ~App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	//Request to save & load
	void LoadRequest();
	void SaveRequest();
	void CapRequest();

private:

	// Load config file
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load & save inside an XML file
	bool LoadGame();
	bool SaveGame();
	bool ChangeCap();

public:

	// Modules
	Window* win;
	Input* input;
	Render* render;
	Textures* tex;
	Fonts* fonts;
	Audio* audio;
	Player* player;
	EnemyManagement* enemies;
	Map* map;
	Transition* transition;
	Collisions* collisions;
	PathFinding* pathfinding;

	Scene* scene;
	LogoScene* logoScene;
	TitleScene* titleScene;
	DeathScene* deathScene;

	bool vsync = false;
	int generalTileSize = 64;

private:

	int argc;
	char** args;
	SString title;
	SString organization;

	List<Module *> modules;

	bool saveRequest, loadRequest, capRequest;
	pugi::xml_document saveFile;
	pugi::xml_node save;

	// Frame variables
	PerfTimer ptimer;
	uint64 frameCount = 0;

	Timer startupTime;
	Timer frameTime;
	Timer lastSecFrameTime;
	uint32 lastSecFrameCount = 0;
	uint32 prevLastSecFrameCount = 0;
	float dt = 0.0f;
	float cappedMs = -1;
	int cap = 0;
};

extern App* app;

#endif	// __APP_H__