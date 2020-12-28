#include "App.h"

#include "Window.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Map.h"
#include "EntityManager.h"
#include "SceneManager.h"

#include "Defs.h"
#include "Log.h"

#include <iostream>
#include <sstream>

// L07: DONE 3: Measure the amount of ms that takes to execute:
// App constructor, Awake, Start and CleanUp
// LOG the result

// Constructor
App::App(int argc, char* args[]) : argc(argc), args(args)
{
	PERF_START(ptimer);

	win = new Window();
	input = new Input(win);
	render = new Render(win);
	tex = new Textures(render);
	audio = new AudioManager();
	map = new Map(tex);
	entityManager = new EntityManager(render, tex, input, map, audio, this);
	sceneManager = new SceneManager(input, render, tex);

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(win);
	AddModule(input);
	AddModule(tex);
	AddModule(audio);
	AddModule(map);
	AddModule(entityManager);
	AddModule(sceneManager);

	// Render last to swap buffer
	AddModule(render);
	
	PERF_PEEK(ptimer);
}

// Destructor
App::~App()
{
	// Release modules
	ListItem<Module*>* item = modules.end;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->prev;
	}

	modules.Clear();
}

void App::AddModule(Module* module)
{
	module->Init();
	modules.Add(module);
}

// Called before render is available
bool App::Awake()
{
	PERF_START(ptimer);

	pugi::xml_document configFile;
	pugi::xml_node config;
	pugi::xml_node configApp;

	bool ret = false;

	// L01: DONE 3: Load config from XML
	config = LoadConfig(configFile);

	if (config.empty() == false)
	{
		ret = true;
		configApp = config.child("app");

		// L01: DONE 4: Read the title from the config file
		title.Create(configApp.child("title").child_value());
		organization.Create(configApp.child("organization").child_value());

        // L08: DONE 1: Read from config file your framerate cap
		cap = configApp.attribute("framerate_cap").as_int(-1);
		if (cap > 0) cappedMs = 1000 / cap;
	}

	if (ret == true)
	{
		ListItem<Module*>* item;
		item = modules.start;

		while ((item != NULL) && (ret == true))
		{
			// L01: DONE 5: Add a new argument to the Awake method to receive a pointer to an xml node.
			// If the section with the module name exists in config.xml, fill the pointer with the valid xml_node
			// that can be used to read all variables for that module.
			// Send nullptr if the node does not exist in config.xml
			ret = item->data->Awake(config.child(item->data->name.GetString()));
			item = item->next;
		}
	}
	
	PERF_PEEK(ptimer);

	return ret;
}

// Called before the first frame
bool App::Start()
{
	PERF_START(ptimer);
	
	bool ret = true;
	ListItem<Module*>* item;
	item = modules.start;

	while(item != NULL && ret == true)
	{
		if (item->data->active == true)
		{
			ret = item->data->Start();
		}
		item = item->next;
	}
	
	capGameRequested = false;

	PERF_PEEK(ptimer);

	return ret;
}

// Called each loop iteration
bool App::Update()
{
	bool ret = true;
	PrepareUpdate();

	if(input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// Load config from XML file
// NOTE: Function has been redesigned to avoid storing additional variables on the class
pugi::xml_node App::LoadConfig(pugi::xml_document& configFile) const
{
	pugi::xml_node ret;

	pugi::xml_parse_result result = configFile.load_file(CONFIG_FILENAME);

	if (result == NULL) LOG("Could not load xml file: %s. pugi error: %s", CONFIG_FILENAME, result.description());
	else ret = configFile.child("config");

	return ret;
}

// ---------------------------------------------
void App::PrepareUpdate()
{
    frameCount++;
    lastSecFrameCount++;

    // L08: DONE 4: Calculate the dt: differential time since last frame
	dt = frameTime.ReadSec();
	frameTime.Start();
}

// ---------------------------------------------
void App::FinishUpdate()
{
	// L02: DONE 1: This is a good place to call Load / Save methods
	if (loadGameRequested == true)
	{
		loadGameRequested = !loadGameRequested;
		LoadGame();
	}
	if (saveGameRequested == true)
	{
		saveGameRequested = !saveGameRequested;
		SaveGame();
	}
	if (capGameRequested == true)
	{
		capGameRequested = !capGameRequested;
		CapRequest();
	}
    
    // L07: DONE 4: Now calculate:
	// Amount of frames since startup
	// Amount of time since game start (use a low resolution timer)
	// Average FPS for the whole game life
	// Amount of ms took the last update
	// Amount of frames during the last second
	if (lastSecFrameTime.Read() > 1000)
	{
		lastSecFrameTime.Start();
		prevLastSecFrameCount = lastSecFrameCount;
		lastSecFrameCount = 0;
	}

	float averageFps = float(frameCount) / startupTime.ReadSec();
	float secondsSinceStartup = startupTime.ReadSec();
	uint32 lastFrameMs = frameTime.Read();
	uint32 framesOnLastUpdate = prevLastSecFrameCount;

	static char title[256];
	sprintf_s(title, 256, "Av.FPS: %.2f Last Frame Ms: %02u Last sec frames: %i Last dt: %.3f Time since startup: %.3f Frame Count: %I64u ",
			  averageFps, lastFrameMs, framesOnLastUpdate, dt, secondsSinceStartup, frameCount);

	//app->win->SetTitle(title);

    // L08: DONE 2: Use SDL_Delay to make sure you get your capped framerate
	if ((cappedMs > 0) && (lastFrameMs < cappedMs))
	{
		// L08: DONE 3: Measure accurately the amount of time SDL_Delay actually waits compared to what was expected
		PerfTimer pt;
		SDL_Delay(cappedMs - lastFrameMs);
		LOG("We waited for %d milliseconds and got back in %f", cappedMs - lastFrameMs, pt.ReadMs());
	}
}

// Call modules before each loop iteration
bool App::PreUpdate()
{
	bool ret = true;

	ListItem<Module*>* item;
	Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool App::DoUpdate()
{
	bool ret = true;
	ListItem<Module*>* item;
	item = modules.start;
	Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

        // L08: DONE 5: Send dt as an argument to all updates, you need
        // to update module parent class and all modules that use update
		ret = item->data->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool App::PostUpdate()
{
	bool ret = true;
	ListItem<Module*>* item;
	Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool App::CleanUp()
{
	bool ret = true;
	ListItem<Module*>* item;
	item = modules.end;

	while(item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	return ret;
}

// ---------------------------------------
int App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* App::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* App::GetTitle() const
{
	return title.GetString();
}

// ---------------------------------------
const char* App::GetOrganization() const
{
	return organization.GetString();
}

// Load / Save
void App::LoadRequest()
{
	// NOTE: We should check if SAVE_STATE_FILENAME actually exist
	loadGameRequested = true;
}

// ---------------------------------------
void App::SaveRequest() const
{
	// NOTE: We should check if SAVE_STATE_FILENAME actually exist and... should we overwriten
	saveGameRequested = true;
}

void App::CapRequest()
{
	capGameRequested = true;
}

// ---------------------------------------
// L02: TODO 5: Create a method to actually load an xml file
// then call all the modules to load themselves
bool App::LoadGame()
{
	bool ret = false;

	pugi::xml_parse_result result = saveFile.load_file("save_game.xml");
	if (result == NULL)
	{
		LOG("Could not load map xml file savegame.xml. pugi error: %s", result.description());
		ret = false;
	}
	else
	{
		save = saveFile.child("save_state");

		ListItem<Module*>* item;
		item = modules.start;

		while (item != NULL && ret == true)
		{
			ret = item->data->Load(save.child(item->data->name.GetString()));
			item = item->next;
		}
	}

	return ret;
}

// L02: TODO 7: Implement the xml save method for current state
bool App::SaveGame() const
{
	bool ret = true;

	pugi::xml_document newSaveFile;
	pugi::xml_node saveState = newSaveFile.append_child("save_state");

	ListItem<Module*>* item;
	item = modules.start;

	while (item != NULL && ret == true)
	{
		ret = item->data->Save(saveState.append_child(item->data->name.GetString()));
		item = item->next;
	}

	newSaveFile.save_file("save_game.xml");

	return ret;
}

bool App::ChangeCap()
{
	bool ret = true;

	switch (cap)
	{
	case 60:
		cap = 30;
		break;
	case 30:
		cap = 60;
		break;
	default:
		break;
	}

	if (cap > 0)
	{
		cappedMs = 1000 / cap;
	}

	return ret;
}