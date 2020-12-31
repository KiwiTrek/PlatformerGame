#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Transition.h"
#include "Defs.h"

#include "LogoScene.h"

#include "Log.h"


LogoScene::LogoScene() : Module()
{
	name.Create("logoScene");
}

LogoScene::~LogoScene()
{}

void LogoScene::Init()
{
	active = true;
}

bool LogoScene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;
	timer = config.child("timer").attribute("value").as_float(0.0f);

	folderTexture.Create(config.child("folderTexture").child_value());
	folderAudioFx.Create(config.child("folderAudioFx").child_value());

	return ret;
}

bool LogoScene::Start()
{
	SString tmp("%s%s", folderTexture.GetString(), "logo_screen.png");
	logo = app->tex->Load(tmp.GetString());
	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "logo.wav");
	logoFx = app->audio->LoadFx(tmp.GetString());
	app->audio->SetFxVolume(logoFx);

	window = { 0,0,0,0 };
	state = 0;
	logoAlpha = 0.0f;

	return true;
}

bool LogoScene::PreUpdate()
{
	return true;
}

bool LogoScene::Update(float dt)
{
	if (state == 0)
	{
		state = 1;
	}
	else if (state == 1)
	{
		logoAlpha += (1.0f * dt);

		if (logoAlpha > 1.0f)
		{
			logoAlpha = 1.0f;
			state = 2;
		}
	}
	else if (state == 2)
	{
		// Waiting for 3 seconds
		timer += dt;
		if (timer >= 0.5f && onceTimer)
		{
			onceTimer = false;
			app->audio->PlayFx(logoFx);
		}
		if (timer >= 2.0f) state = 3;
	}
	else if (state == 3)
	{
		logoAlpha -= (1.0f * dt);

		if (logoAlpha < 0.0f)
		{
			logoAlpha = 0.0f;
			state = 4;
		}
	}
	else if (state == 4)
	{
		app->transition->FadeEffect(this, (Module*)app->titleScene, false, floor(1200.0f * dt));
	}

	return true;
}

bool LogoScene::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		ret = false;
	}
	
	uint w, h;
	app->win->GetWindowSize(w, h);
	window.w = (int)w;
	window.h = (int)h;

	app->render->DrawRectangle(window, 255, 255, 255, 255);
	app->render->DrawTexture(logo, NULL, NULL, true);
	app->render->DrawRectangle(window, 255, 255, 255, (uchar)(255 - (255 * logoAlpha)));

	return ret;
}

bool LogoScene::CleanUp()
{
	LOG("Freeing scene");

	app->tex->UnLoad(logo);
	app->audio->UnloadFx(logoFx);

	return true;
}