#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "TitleScene.h"
#include "DeathScene.h"
#include "Transition.h"
#include "Scene.h"

#include "Defs.h"
#include "Log.h"

DeathScene::DeathScene() : Module()
{
	name.Create("deathScene");
}

// Destructor
DeathScene::~DeathScene()
{}

void DeathScene::Init()
{
	active = false;
}

// Called before render is available
bool DeathScene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool DeathScene::Start()
{
	app->render->SetBackgroundColor({ 0,0,0,0 });
	deathScreen = app->tex->Load("Assets/textures/gameOver.png");
	app->audio->PlayMusic("Assets/audio/music/GameOver.ogg", 0.5f);
	return true;
}

// Called each loop iteration
bool DeathScene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool DeathScene::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		app->transition->FadeEffect(this, (Module*)app->titleScene, false, 30.0f);
	}
	return true;
}

// Called each loop iteration
bool DeathScene::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	app->render->DrawTexture(deathScreen, NULL, NULL, true);

	return ret;
}

// Called before quitting
bool DeathScene::CleanUp()
{
	LOG("Freeing scene");

	app->tex->UnLoad(deathScreen);

	return true;
}
