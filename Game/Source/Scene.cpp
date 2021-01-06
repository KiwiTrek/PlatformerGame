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
#include "TitleScene.h"
#include "DeathScene.h"
#include "PathFinding.h"

#include "Scene.h"

#include "Defs.h"
#include "Log.h"

Scene::Scene() : Module()
{
	name.Create("scene");
}

Scene::~Scene()
{}

void Scene::Init()
{
	active = false;
}

bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");

	folderTexture.Create(config.child("folderTexture").child_value());
	folderAudioMusic.Create(config.child("folderAudioMusic").child_value());

	return true;
}

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

	scoreValue = 0;
	fruitCounter = 0;
	coinCounter = 0;

	app->collisions->Enable();
	app->entities->Enable();

	player = app->entities->CreateEntity(-1, -1, EntityType::PLAYER);
	app->entities->CreateEntity(app->map->data.tileWidth * 103, app->map->data.tileHeight * 3, EntityType::ENEMY, player, EnemyType::FLYING);
	app->entities->CreateEntity(app->map->data.tileWidth * 36, app->map->data.tileHeight * 8, EntityType::ENEMY, player, EnemyType::GROUND);
	app->entities->CreateEntity(app->map->data.tileWidth * 29, app->map->data.tileHeight * 13, EntityType::FRUIT);
	app->entities->CreateEntity(app->map->data.tileWidth * 63, app->map->data.tileHeight * 1, EntityType::FRUIT);
	app->entities->CreateEntity(app->map->data.tileWidth * 80, app->map->data.tileHeight * 8, EntityType::FRUIT);
	app->entities->CreateEntity(app->map->data.tileWidth * 102, app->map->data.tileHeight * 12, EntityType::FRUIT);
	app->entities->CreateEntity(app->map->data.tileWidth * 118, app->map->data.tileHeight * 9, EntityType::FRUIT);
	for (int i = 0; i != 6; i++)
	{
		int tmp = 0;
		switch (i)
		{
		case 0:
			tmp = 0;
			break;
		case 1:
			tmp = 1;
			break;
		case 2:
			tmp = 8;
			break;
		case 3:
			tmp = 9;
			break;
		case 4:
			tmp = 10;
			break;
		case 5:
			tmp = 11;
			break;
		default:
			break;
		}
		app->entities->CreateEntity(app->map->data.tileWidth * (70 + tmp), app->map->data.tileHeight * 2, EntityType::COIN);
	}
	for (int i = 0; i != 8; i++)
	{
		int tmp = 0;
		switch (i)
		{
		case 0:
			tmp = 0;
			break;
		case 1:
			tmp = 1;
			break;
		case 2:
			tmp = 2;
			break;
		case 3:
			tmp = 3;
			break;
		case 4:
			tmp = 18;
			break;
		case 5:
			tmp = 30;
			break;
		case 6:
			tmp = 31;
			break;
		case 7:
			tmp = 32;
			break;
		default:
			break;
		}
		app->entities->CreateEntity(app->map->data.tileWidth * (56 + tmp), app->map->data.tileHeight * 4, EntityType::COIN);
	}
	for (int i = 0; i != 7; i++)
	{
		int tmp = 0;
		switch (i)
		{
		case 0:
			tmp = 0;
			break;
		case 1:
			tmp = 1;
			break;
		case 2:
			tmp = 2;
			break;
		case 3:
			tmp = 3;
			break;
		case 4:
			tmp = 4;
			break;
		case 5:
			tmp = 5;
			break;
		case 6:
			tmp = 19;
			break;
		default:
			break;
		}
		app->entities->CreateEntity(app->map->data.tileWidth * (55 + tmp), app->map->data.tileHeight * 5, EntityType::COIN);
	}
	for (int i = 0; i != 10; i++)
	{
		int tmp = 0;
		switch (i)
		{
		case 0:
			tmp = 0;
			break;
		case 1:
			tmp = 1;
			break;
		case 2:
			tmp = 2;
			break;
		case 3:
			tmp = 13;
			break;
		case 4:
			tmp = 14;
			break;
		case 5:
			tmp = 15;
			break;
		case 6:
			tmp = 39;
			break;
		case 7:
			tmp = 68;
			break;
		case 8:
			tmp = 69;
			break;
		case 9:
			tmp = 73;
			break;
		default:
			break;
		}
		app->entities->CreateEntity(app->map->data.tileWidth * (35 + tmp), app->map->data.tileHeight * 6, EntityType::COIN);
	}
	for (int i = 0; i != 7; i++)
	{
		int tmp = 0;
		switch (i)
		{
		case 0:
			tmp = 0;
			break;
		case 1:
			tmp = 7;
			break;
		case 2:
			tmp = 8;
			break;
		case 3:
			tmp = 9;
			break;
		case 4:
			tmp = 19;
			break;
		case 5:
			tmp = 23;
			break;
		case 6:
			tmp = 46;
			break;
		default:
			break;
		}
		app->entities->CreateEntity(app->map->data.tileWidth * (28 + tmp), app->map->data.tileHeight * 7, EntityType::COIN);
	}
	for (int i = 0; i != 3; i++)
	{
		int tmp = 0;
		switch (i)
		{
		case 0:
			tmp = 0;
			break;
		case 1:
			tmp = 1;
			break;
		case 2:
			tmp = 56;
			break;
		default:
			break;
		}
		app->entities->CreateEntity(app->map->data.tileWidth * (18 + tmp), app->map->data.tileHeight * 8, EntityType::COIN);
	}
	for (int i = 0; i != 3; i++)
	{
		int tmp = 0;
		switch (i)
		{
		case 0:
			tmp = 0;
			break;
		case 1:
			tmp = 81;
			break;
		case 2:
			tmp = 82;
			break;
		default:
			break;
		}
		app->entities->CreateEntity(app->map->data.tileWidth * (17 + tmp), app->map->data.tileHeight * 9, EntityType::COIN);
	}
	app->entities->CreateEntity(app->map->data.tileWidth * 93, app->map->data.tileHeight * 10, EntityType::COIN);
	app->entities->CreateEntity(app->map->data.tileWidth * 94, app->map->data.tileHeight * 10, EntityType::COIN);
	for (int i = 0; i != 7; i++)
	{
		int tmp = 0;
		switch (i)
		{
		case 0:
			tmp = 0;
			break;
		case 1:
			tmp = 1;
			break;
		case 2:
			tmp = 2;
			break;
		case 3:
			tmp = 26;
			break;
		case 4:
			tmp = 27;
			break;
		case 5:
			tmp = 28;
			break;
		case 6:
			tmp = 75;
			break;
		default:
			break;
		}
		app->entities->CreateEntity(app->map->data.tileWidth * (9 + tmp), app->map->data.tileHeight * 11, EntityType::COIN);
	}
	for (int i = 0; i != 7; i++)
	{
		int tmp = 0;
		switch (i)
		{
		case 0:
			tmp = 0;
			break;
		case 1:
			tmp = 1;
			break;
		case 2:
			tmp = 2;
			break;
		default:
			break;
		}
		app->entities->CreateEntity(app->map->data.tileWidth * (108 + tmp), app->map->data.tileHeight * 12, EntityType::COIN);
	}

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

	cameraPos = { -app->render->camera.x, -app->render->camera.y };
	cameraSize = { app->render->camera.w, app->render->camera.h };

	if (app->titleScene->loadRequest)
	{
		app->titleScene->loadRequest = false;
		app->LoadRequest();
	}

	return true;
}

bool Scene::PreUpdate()
{
	return true;
}

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

	if (app->entities->exitRequest)
	{
		return false;
	}

	// DEBUG
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

	// Player camera restraint
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

bool Scene::PostUpdate()
{
	cameraPos = { -app->render->camera.x, -app->render->camera.y };

	// Background
	uint w, h;
	app->win->GetWindowSize(w, h);
	uint wmb, hmb;
	app->tex->GetSize(mountainsBack, wmb, hmb);
	for (int i = 0; (wmb * i) <= (w - app->render->camera.x); i++)
	{
		app->render->DrawTexture(mountainsBack, wmb * i, app->map->data.tileHeight * 7, false, nullptr, false, 0.4f);
		app->render->DrawTexture(clouds, wmb * i, app->map->data.tileHeight * 2, false, nullptr, false, 0.5f);
		app->render->DrawTexture(mountainsFront, wmb * i, app->map->data.tileHeight * 9, false, nullptr, false, 0.85f);
	}

	app->map->Draw();

	// Fruit & Coin
	sprintf_s(scoreFruit, 4, "%2d", fruitCounter);
	sprintf_s(scoreCoin, 4, "%02d", coinCounter);
	app->render->DrawTexture(ui, cameraPos.x + 2, cameraPos.y + 64, false, &uiCoin);
	app->fonts->DrawText(cameraPos.x + 8 + 36, cameraPos.y + 62, font, scoreCoin);
	app->render->DrawTexture(ui, cameraPos.x, cameraPos.y + 100, false, &uiFruit);
	app->fonts->DrawText(cameraPos.x + 8, cameraPos.y + 98, font, scoreFruit);

	// Score
	sprintf_s(score, 6, "%03d", scoreValue);
	app->fonts->DrawText(cameraPos.x + cameraSize.x - offsetScore - 12, cameraPos.y + 36, font, scoreTitle.GetString());
	app->fonts->DrawText(cameraPos.x + cameraSize.x - (36 * 3) - 30, cameraPos.y + 36, font, ":");
	app->fonts->DrawText(cameraPos.x + cameraSize.x - (36 * 3), cameraPos.y + 36, font, score);

	// Timer
	sprintf_s(timer, 6, "%03d", (int)timerValue);
	app->fonts->DrawText(cameraPos.x + cameraSize.x - offsetTimer - 12, cameraPos.y + 2, font, timerTitle.GetString());
	app->fonts->DrawText(cameraPos.x + cameraSize.x - (36 * 3) - 30, cameraPos.y + 2, font, ":");
	app->fonts->DrawText(cameraPos.x + cameraSize.x - (36 * 3), cameraPos.y + 2, font, timer);

	return true;
}

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

	scoreValue = save.child("score").attribute("value").as_int();
	coinCounter = save.child("coin").attribute("value").as_int();
	fruitCounter = save.child("fruit").attribute("value").as_int();
	timerValue = save.child("timer").attribute("value").as_float(500.0f);

	// Store the current player entity
	ListItem<Entity*>* e = app->entities->entities.start;
	while (e != nullptr)
	{
		if (e->data->type == EntityType::PLAYER)
		{
			player = e->data;
		}
		e = e->next;
	}

	return true;
}

bool Scene::Save(pugi::xml_node& save)
{
	LOG("Saving scene variables");

	save.append_child("score").append_attribute("value").set_value(scoreValue);
	save.append_child("coin").append_attribute("value").set_value(coinCounter);
	save.append_child("fruit").append_attribute("value").set_value(fruitCounter);
	save.append_child("timer").append_attribute("value").set_value(timerValue);

	return true;
}