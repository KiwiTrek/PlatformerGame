#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Map.h"

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
	sky = app->tex->Load("Assets/textures/Background/sky_solid_color.png");
	clouds = app->tex->Load("Assets/textures/Background/clouds.png");
	mountainsBack = app->tex->Load("Assets/textures/Background/mountain_depth_z_1.png");
	mountainsFront = app->tex->Load("Assets/textures/Background/mountain_depth_z_2.png");
	app->audio->PlayMusic("Assets/audio/music/music_spy.ogg");
	app->map->Load("level1.tmx");
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

	app->render->camera.y = app->map->data.tileH*-2;
	app->render->DrawTexture(sky, 0, 0, true, nullptr, 0.0f);

	uint w, h;
	app->win->GetWindowSize(w, h);
	uint wmb, hmb;
	app->tex->GetSize(mountainsBack, wmb, hmb);
	//for (int i = 0; (wmb * i) <= (w - app->render->camera.x); i++)
	//{
	//	app->render->DrawTexture(mountainsBack, wmb*i, app->map->data.tileH * 6, false, nullptr, 0.5f);
	//	LOG("%d, %d", i, -app->render->camera.x);
	//}
	app->render->DrawTexture(mountainsBack, 0, app->map->data.tileH * 6, false, nullptr, 0.5f);
	app->render->DrawTexture(clouds, 0, app->map->data.tileH * 2, false, nullptr, 0.6f);
	app->render->DrawTexture(mountainsFront, 0, app->map->data.tileH * 8, false, nullptr, 0.85f);
	app->map->Draw();

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
