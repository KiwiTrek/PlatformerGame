#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Fonts.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "EntityManager.h"
#include "GuiManager.h"
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
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "ui_elements.png");
	ui = app->tex->Load(tmp.GetString());
	uiCoin = { 0,0,32,32 };
	uiFruit = { 32,0,32,32 };

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
	player = app->entities->CreateEntity(-1, -1, EntityType::PLAYER);
	app->entities->CreateEntity(app->map->data.tileWidth * 103, app->map->data.tileHeight * 3, EntityType::ENEMY, player, EnemyType::FLYING);
	app->entities->CreateEntity(app->map->data.tileWidth * 36, app->map->data.tileHeight * 8, EntityType::ENEMY, player, EnemyType::GROUND);
	app->entities->CreateEntity(app->map->data.tileWidth * 30, app->map->data.tileHeight * 7, EntityType::COIN);

	app->render->camera.x = -(player->spawnPos.x - app->render->camera.w / 2);
	app->render->camera.y = -(player->spawnPos.y - app->render->camera.h / 2 - 64);

	tmp.Clear();
	tmp.Create("%s%s", folderAudioMusic.GetString(), "level_1.ogg");
	app->audio->PlayMusic(tmp.GetString(), 0.0f);

	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "score_font.png");
	font = app->fonts->Load(tmp.GetString(), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,_-@#*^()[]<>: ", 3);

	scoreTitle.Create("Score");
	offsetScore = scoreTitle.Length() * 36 + (36 * 3);

	timerValue = 501.0f;
	timerTitle.Create("Time");
	offsetTimer = timerTitle.Length() * 36 + (36 * 3);

	fruitCounter = 0;
	coinCounter = 0;

	cameraPos = { -app->render->camera.x, -app->render->camera.y };
	cameraSize = { app->render->camera.w, app->render->camera.h };

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
	if (app->entities->doLogic)
	{
		timerValue -= dt;
		if (timerValue <= 0.0f)
		{
			timerValue = 0.0f;
			player->isDead = true;
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		app->audio->MuteVolume();
	}

	if (app->entities->exitRequest)
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
	cameraPos = { -app->render->camera.x, -app->render->camera.y };

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

	sprintf_s(scoreFruit, 4, "%2d", fruitCounter);
	sprintf_s(scoreCoin, 4, "%2d", coinCounter);
	app->render->DrawTexture(ui, cameraPos.x + 2, cameraPos.y + 64, false, &uiCoin);
	app->fonts->DrawText(cameraPos.x + 8, cameraPos.y + 62, font, scoreCoin);
	app->render->DrawTexture(ui, cameraPos.x, cameraPos.y + 100, false, &uiFruit);
	app->fonts->DrawText(cameraPos.x + 8, cameraPos.y + 98, font, scoreFruit);

	sprintf_s(score, 6, "%03d", scoreValue);
	app->fonts->DrawText(cameraPos.x + cameraSize.x - offsetScore - 12, cameraPos.y + 36, font, scoreTitle.GetString());
	app->fonts->DrawText(cameraPos.x + cameraSize.x - (36 * 3) - 30, cameraPos.y + 36, font, ":");
	app->fonts->DrawText(cameraPos.x + cameraSize.x - (36 * 3), cameraPos.y + 36, font, score);

	sprintf_s(timer, 6, "%03d", (int)timerValue);
	app->fonts->DrawText(cameraPos.x + cameraSize.x - offsetTimer - 12, cameraPos.y + 2, font, timerTitle.GetString());
	app->fonts->DrawText(cameraPos.x + cameraSize.x - (36 * 3) - 30, cameraPos.y + 2, font, ":");
	app->fonts->DrawText(cameraPos.x + cameraSize.x - (36 * 3), cameraPos.y + 2, font, timer);

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
	app->tex->UnLoad(ui);
	app->fonts->Unload(font);

	app->entities->Disable();
	app->map->Disable();

	return true;
}

bool Scene::Load(pugi::xml_node& save)
{
	LOG("Loading scene variables");
	bool ret = true;

	scoreValue = save.child("score").attribute("value").as_int();
	coinCounter = save.child("coin").attribute("value").as_int();
	fruitCounter = save.child("fruit").attribute("value").as_int();
	timerValue = save.child("timer").attribute("value").as_float(500.0f);

	ListItem<Entity*>* e = app->entities->entities.start;
	while (e != nullptr)
	{
		if (e->data->type == EntityType::PLAYER)
		{
			player = e->data;
		}
		e = e->next;
	}

	return ret;
}

bool Scene::Save(pugi::xml_node& save)
{
	LOG("Saving scene variables");
	bool ret = true;

	save.append_child("score").append_attribute("value").set_value(scoreValue);
	save.append_child("coin").append_attribute("value").set_value(coinCounter);
	save.append_child("fruit").append_attribute("value").set_value(fruitCounter);
	save.append_child("timer").append_attribute("value").set_value(timerValue);

	return ret;
}