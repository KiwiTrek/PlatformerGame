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
	app->transition->FadeEffect(nullptr, this, true, 120.0f);
	logo = app->tex->Load("Assets/textures/logoScreen.jpg");
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
	if (timer < 100)
	{
		timer++;
	}
	else
	{
		app->transition->FadeEffect(this, (Module*)app->titleScene, false, 45);
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

	return true;
}