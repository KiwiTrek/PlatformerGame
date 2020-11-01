#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Map.h"
#include "Player.h"
#include "Transition.h"
#include "DeathScene.h"

#include "Defs.h"
#include "Log.h"

Scene::Scene() : Module()
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

void Scene::Init()
{
	active = false;
}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	folderTexture.Create(config.child("folderTexture").child_value());
	folderAudioMusic.Create(config.child("folderAudioMusic").child_value());

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	SString tmp("%s%s", folderTexture.GetString(), "clouds.png");
	clouds = app->tex->Load(tmp.GetString());
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "mountain_depth_back.png");
	mountainsBack = app->tex->Load(tmp.GetString());
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "mountain_depth_front.png");
	mountainsFront = app->tex->Load(tmp.GetString());

	app->map->Enable();
	app->map->Load("level1.tmx");
	app->render->SetBackgroundColor(app->map->data.backgroundColor);

	app->render->camera.x = 0;
	app->render->camera.y = app->map->data.tileH * -2; // -128

	app->player->Enable();

	tmp.Clear();
	tmp.Create("%s%s", folderAudioMusic.GetString(), "Level1.ogg");
	app->audio->PlayMusic(tmp.GetString(), 0.0f);

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		app->audio->MuteVolume();
	}

	//DEBUG
	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		app->transition->FadeEffect(this, (Module*)app->scene, false);
	}
	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		app->transition->FadeEffect(this, this, false);
	}

	if (app->render->drawAll)
	{
		SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d", app->map->data.w, app->map->data.h, app->map->data.tileW, app->map->data.tileH, app->map->data.tileSets.count());
		app->win->SetTitle(title.GetString());
	}
	else
	{
		app->win->SetTitle(app->GetTitle());
	}

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	//Player restraint
	if ((app->render->camera.x + app->player->playerRect.x) < (app->map->data.tileW * 6))
	{
		app->render->camera.x += 5;
	}
	if ((app->player->playerRect.w + app->render->camera.x + app->player->playerRect.x) > (app->render->camera.w - app->map->data.tileW * 10))
	{
		app->render->camera.x -= 5;
	}
	if ((app->render->camera.y + app->player->playerRect.y) < (app->map->data.tileH * 6))
	{
		app->render->camera.y += 5;
	}
	if ((app->player->playerRect.h + app->render->camera.y + app->player->playerRect.y) > (app->render->camera.h - app->map->data.tileH * 6))
	{
		app->render->camera.y -= 5;
	}

	// Map borders
	if (app->render->camera.x >= 0)
	{
		app->render->camera.x -= 5;
	}
	if ((app->render->camera.w - app->render->camera.x) > (app->map->data.w * app->map->data.tileW))
	{
		app->render->camera.x += 5;
	}
	if (app->render->camera.y >= 0)
	{
		app->render->camera.y -= 5;
	}
	if ((app->render->camera.h - app->render->camera.y) > (app->map->data.h * app->map->data.tileH))
	{
		app->render->camera.y += 5;
	}

	uint w, h;
	app->win->GetWindowSize(w, h);
	uint wmb, hmb;
	app->tex->GetSize(mountainsBack, wmb, hmb);
	for (int i = 0; (wmb * i) <= (w - app->render->camera.x); i++)
	{
		app->render->DrawTexture(mountainsBack, wmb * i, app->map->data.tileH * 7, false, nullptr, false, 0.4f);
		app->render->DrawTexture(clouds, wmb * i, app->map->data.tileH * 3, false, nullptr, false, 0.5f);
		app->render->DrawTexture(mountainsFront, wmb * i, app->map->data.tileH * 9, false, nullptr, false, 0.85f);
	}

	app->map->Draw();

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	app->render->camera.x = 0;
	app->render->camera.y = 0;

	app->tex->UnLoad(clouds);
	app->tex->UnLoad(mountainsBack);
	app->tex->UnLoad(mountainsFront);

	app->player->Disable();
	app->map->Disable();

	return true;
}
