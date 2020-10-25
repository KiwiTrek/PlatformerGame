#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Map.h"
#include "Player.h"

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
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	clouds = app->tex->Load("Assets/textures/Background/clouds.png");
	mountainsBack = app->tex->Load("Assets/textures/Background/mountain_depth_z_1.png");
	mountainsFront = app->tex->Load("Assets/textures/Background/mountain_depth_z_2.png");
	app->audio->PlayMusic("Assets/audio/music/music_spy.ogg");
	app->map->Load("level1.tmx");

	app->render->camera.x = 0;
	app->render->camera.y = app->map->data.tileH * -2;

	app->player->Enable();

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

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		app->LoadRequest();
	}
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		app->SaveRequest();
	}
	if (app->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		app->audio->MuteVolume();
	}
	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		app->render->camera.y += 1;
	}
	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		app->render->camera.y -= 1;
	}
	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		app->render->camera.x += 1;
	}
	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		app->render->camera.x -= 1;
	}

	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d", app->map->data.w, app->map->data.h, app->map->data.tileW, app->map->data.tileH, app->map->data.tilesets.count());

	app->win->SetTitle(title.GetString());

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;


	LOG("%d,%d", app->render->camera.x, app->render->camera.y);

	//Player restraint
	if ((app->render->camera.x + app->player->playerRect.x) < (app->map->data.tileW * 6))
	{
		++app->render->camera.x;
	}
	if ((app->player->playerRect.w + app->render->camera.x + app->player->playerRect.x) > (app->render->camera.w - app->map->data.tileW * 10))
	{
		--app->render->camera.x;
	}
	if ((app->render->camera.y + app->player->playerRect.y) < (app->map->data.tileH * 6))
	{
		++app->render->camera.y;
	}
	if ((app->player->playerRect.h + app->render->camera.y + app->player->playerRect.y) > (app->render->camera.h - app->map->data.tileH * 6))
	{
		--app->render->camera.y;
	}

	// Map borders
	if (app->render->camera.x >= 0)
	{
		--app->render->camera.x;
	}
	if ((app->render->camera.w - app->render->camera.x) > (app->map->data.w * app->map->data.tileW))
	{
		++app->render->camera.x;
	}
	if (app->render->camera.y >= 0)
	{
		--app->render->camera.y;
	}
	if ((app->render->camera.h - app->render->camera.y) > (app->map->data.h * app->map->data.tileH))
	{
		++app->render->camera.y;
	}

	uint w, h;
	app->win->GetWindowSize(w, h);
	uint wmb, hmb;
	app->tex->GetSize(mountainsBack, wmb, hmb);
	for (int i = 0; (wmb * i) <= (w - app->render->camera.x); i++)
	{
		app->render->DrawTexture(mountainsBack, wmb * i, app->map->data.tileH * 6, false, nullptr, 0.4f);
		app->render->DrawTexture(clouds, wmb * i, app->map->data.tileH * 2.5, false, nullptr, 0.5f);
		app->render->DrawTexture(mountainsFront, wmb * i, app->map->data.tileH * 8, false, nullptr, 0.85f);
	}

	app->map->Draw();

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
