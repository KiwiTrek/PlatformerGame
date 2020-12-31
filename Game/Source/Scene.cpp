#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Fonts.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "EntityManager.h"
#include "Collisions.h"
#include "Transition.h"
#include "DeathScene.h"
#include "PathFinding.h"

#include "Scene.h"

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
	if (app->map->Load("level_1.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;

		if (app->map->CreateWalkabilityMap(&w, &h, &data))
		{
			app->pathfinding->SetMap(w, h, data);
		}

		RELEASE_ARRAY(data);
	}

	app->render->SetBackgroundColor(app->map->data.backgroundColor);

	app->collisions->Enable();

	scoreValue = 0;

	app->entities->Enable();
	player = app->entities->CreateEntity(0, 0, EntityType::PLAYER);
	app->entities->CreateEntity(app->map->data.tileWidth * 103, app->map->data.tileHeight * 3, EntityType::ENEMY, player, EnemyType::FLYING);
	app->entities->CreateEntity(app->map->data.tileWidth * 36, app->map->data.tileHeight * 8, EntityType::ENEMY, player, EnemyType::GROUND);

	app->render->camera.x = -(player->spawnPos.x - app->render->camera.w / 2);
	app->render->camera.y = -(player->spawnPos.y - app->render->camera.h / 2 - 64);


	tmp.Clear();
	tmp.Create("%s%s", folderAudioMusic.GetString(), "level_1.ogg");
	app->audio->PlayMusic(tmp.GetString(), 0.0f);

	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "score_font.png");
	font = app->fonts->Load(tmp.GetString(), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,_-@#*^()[]<>: ", 3);

	scoreTitle.Create("Score:");
	offsetScore = scoreTitle.Length() * 36;

	timerValue = 501.0f;
	timerTitle.Create("Time:");
	offsetTimer = timerTitle.Length() * 36;

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
	timerValue -= dt;
	if (timerValue <= 0.0f)
	{
		timerValue = 0.0f;
		player->isDead = true;
	}
	if (app->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		app->audio->MuteVolume();
	}

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		return false;
	}

	//DEBUG
	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		player->spawnPos = { 0,0 };
		app->transition->FadeEffect(this, (Module*)app->scene, false);
	}
	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		player->spawnPos = { 0,0 };
		app->transition->FadeEffect(this, this, false);
	}

	//Player restraint
	if ((app->render->camera.x + player->entityRect.x) < (app->map->data.tileWidth * 6))
	{
		app->render->camera.x += (250.0f * dt);
	}
	if ((player->entityRect.w + app->render->camera.x + player->entityRect.x) > (app->render->camera.w - app->map->data.tileWidth * 10))
	{
		app->render->camera.x -= (250.0f * dt);
	}
	if ((app->render->camera.y + player->entityRect.y) < (app->map->data.tileHeight * 6))
	{
		app->render->camera.y += floor(250.0f * dt);
	}
	if ((player->entityRect.h + app->render->camera.y + player->entityRect.y) > (app->render->camera.h - app->map->data.tileHeight * 6))
	{
		app->render->camera.y -= floor(250.0f * dt);
	}

	// Map borders
	if (app->render->camera.x >= 0)
	{
		app->render->camera.x = 0;
	}
	if ((app->render->camera.w - app->render->camera.x) > (app->map->data.width * app->map->data.tileWidth))
	{
		app->render->camera.x += floor(250.0f * dt);
	}
	if (app->render->camera.y >= 0)
	{
		app->render->camera.y = 0;
	}
	if ((app->render->camera.h - app->render->camera.y) > (app->map->data.height * app->map->data.tileHeight))
	{
		app->render->camera.y += floor(250.0f * dt);
	}

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	uint w, h;
	app->win->GetWindowSize(w, h);
	uint wmb, hmb;
	app->tex->GetSize(mountainsBack, wmb, hmb);
	for (int i = 0; (wmb * i) <= (w - app->render->camera.x); i++)
	{
		app->render->DrawTexture(mountainsBack, wmb * i, app->map->data.tileHeight * 7, false, nullptr, false, 0.4f);
		app->render->DrawTexture(clouds, wmb * i, app->map->data.tileHeight * 3, false, nullptr, false, 0.5f);
		app->render->DrawTexture(mountainsFront, wmb * i, app->map->data.tileHeight * 9, false, nullptr, false, 0.85f);
	}

	app->map->Draw();
	
	iPoint tmp(-app->render->camera.x, -app->render->camera.y);
	app->render->DrawRectangle({ tmp.x, tmp.y + 102, 288, 5 }, 0, 0, 0, 255);

	sprintf_s(score, 8, "%d", scoreValue);
	app->fonts->DrawText(tmp.x, tmp.y + 66, font, scoreTitle.GetString());
	app->fonts->DrawText(tmp.x + offsetScore, tmp.y + 66, font, score);

	//Would like it on top right side of the screen ;w;
	sprintf_s(timer, 6, "%03d", (int)timerValue);
	app->fonts->DrawText(tmp.x, tmp.y + 106, font, timerTitle.GetString());
	app->fonts->DrawText(tmp.x + offsetTimer, tmp.y + 106, font, timer);

	return true;
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
	app->fonts->Unload(font);

	app->entities->Disable();
	app->map->Disable();

	return true;
}