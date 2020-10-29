#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "TitleScene.h"
#include "Transition.h"
#include "Scene.h"

#include "Defs.h"
#include "Log.h"

TitleScene::TitleScene() : Module()
{
	name.Create("titleScene");
}

// Destructor
TitleScene::~TitleScene()
{}

void TitleScene::Init()
{
	active = false;
}

// Called before render is available
bool TitleScene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool TitleScene::Start()
{
	app->render->SetBackgroundColor({ 0,0,0,0 });
	titleScreen = app->tex->Load("Assets/textures/titleScreen.png");
//	app->audio->PlayMusic("Assets/audio/music/TitleScreen.ogg",0.0f);
	return true;
}

// Called each loop iteration
bool TitleScene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool TitleScene::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		app->transition->FadeEffect(this, (Module*)app->scene, false, 60.0f);
	}
	return true;
}

// Called each loop iteration
bool TitleScene::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	app->render->DrawTexture(titleScreen, NULL, NULL, true);

	return ret;
}

// Called before quitting
bool TitleScene::CleanUp()
{
	LOG("Freeing scene");

	app->tex->UnLoad(titleScreen);

	return true;
}
