#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "LogoScene.h"
#include "Transition.h"

#include "Defs.h"
#include "Log.h"

LogoScene::LogoScene() : Module()
{
	name.Create("logoScene");
}

// Destructor
LogoScene::~LogoScene()
{}

void LogoScene::Init()
{
	active = true;
}

// Called before render is available
bool LogoScene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool LogoScene::Start()
{
	app->transition->FadeEffect(nullptr, this, true, 6000.0f);
	logo = app->tex->Load("Assets/textures/logoScreen.jpg");
	logoFx = app->audio->LoadFx("Assets/audio/fx/logo.wav");
	return true;
}

// Called each loop iteration
bool LogoScene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool LogoScene::Update(float dt)
{
	if (timer < 10000)
	{
		timer++;
	}
	else
	{
		app->transition->FadeEffect(this, (Module*)app->titleScene, false, 6000.0f);
	}

	if (timer == 3000)
	{
	//	app->audio->PlayFx(logoFx);
	}
	return true;
}

// Called each loop iteration
bool LogoScene::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	app->render->DrawTexture(logo, NULL, NULL, true);

	return ret;
}

// Called before quitting
bool LogoScene::CleanUp()
{
	LOG("Freeing scene");

	app->tex->UnLoad(logo);

	app->audio->UnloadFx(logoFx);

	return true;
}