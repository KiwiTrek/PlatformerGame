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

DeathScene::~DeathScene()
{}

void DeathScene::Init()
{
	active = false;
}

bool DeathScene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	folderTexture.Create(config.child("folderTexture").child_value());
	folderAudioMusic.Create(config.child("folderAudioMusic").child_value());

	return ret;
}

bool DeathScene::Start()
{
	app->render->SetBackgroundColor({ 0,0,0,0 });
	SString tmp("%s%s", folderTexture.GetString(), "game_over.png");
	deathScreen = app->tex->Load(tmp.GetString());
	tmp.Clear();
	tmp.Create("%s%s", folderAudioMusic.GetString(), "game_over.ogg");
	app->audio->PlayMusic(tmp.GetString(), 0.5f);

	return true;
}

bool DeathScene::PreUpdate()
{
	return true;
}

bool DeathScene::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		app->transition->FadeEffect(this, (Module*)app->titleScene, false, floor(1200.0f * dt));
	}
	return true;
}

bool DeathScene::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		ret = false;
	}

	app->render->DrawTexture(deathScreen, NULL, NULL, true);

	return ret;
}

bool DeathScene::CleanUp()
{
	LOG("Freeing scene");

	app->tex->UnLoad(deathScreen);

	return true;
}