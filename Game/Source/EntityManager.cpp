#include "EntityManager.h"

#include "App.h"

#include "Player.h"
#include "Enemy.h"
#include "Collisions.h"
#include "Map.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "PathFinding.h"

#include "Defs.h"
#include "Log.h"

EntityManager::EntityManager() : Module()
{
	name.Create("entityManager");
}

EntityManager::~EntityManager()
{}

void EntityManager::Init()
{
	active = false;
}

bool EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Loading Entity Manager");
	bool ret = true;

	folderAudioFx.Create(config.child("folderAudioFx").child_value());
	folderTexture.Create(config.child("folderTexture").child_value());

	return ret;
}

bool EntityManager::Start()
{
	// Fx
	SString tmp("%s%s", folderAudioFx.GetString(), "enemy_death.wav");
	enemyDestroyedFx = app->audio->LoadFx(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "lose.wav");
	deadFx = app->audio->LoadFx(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "jump.wav");
	jumpFx = app->audio->LoadFx(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "double_jump.wav");
	doubleJumpFx = app->audio->LoadFx(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "fruit.wav");
	fruitFx = app->audio->LoadFx(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "hit.wav");
	hitFx = app->audio->LoadFx(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "slash.wav");
	slashFx = app->audio->LoadFx(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "checkpoint.wav");
	checkpointFx = app->audio->LoadFx(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "enemy_death.wav");
	enemyDestroyedFx = app->audio->LoadFx(tmp.GetString());

	app->audio->SetFxVolume(deadFx);
	app->audio->SetFxVolume(jumpFx);
	app->audio->SetFxVolume(doubleJumpFx);
	app->audio->SetFxVolume(fruitFx);
	app->audio->SetFxVolume(hitFx);
	app->audio->SetFxVolume(slashFx);
	app->audio->SetFxVolume(checkpointFx);

	// Tex
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "character_spritesheet.png");
	playerTex = app->tex->Load(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "heart.png");
	playerHeart = app->tex->Load(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "enemy_ground_spritesheet.png");
	ground = app->tex->Load(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "enemy_flying_spritesheet.png");
	flying = app->tex->Load(tmp.GetString());

	return true;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	if (!active) return true;
	app->tex->UnLoad(playerTex);
	app->tex->UnLoad(ground);
	app->tex->UnLoad(flying);

	app->audio->UnloadFx(deadFx);
	app->audio->UnloadFx(doubleJumpFx);
	app->audio->UnloadFx(fruitFx);
	app->audio->UnloadFx(jumpFx);
	app->audio->UnloadFx(hitFx);
	app->audio->UnloadFx(slashFx);
	app->audio->UnloadFx(checkpointFx);
	app->audio->UnloadFx(enemyDestroyedFx);

	return true;
}

Entity* EntityManager::CreateEntity(int x, int y, EntityType type, EnemyType eType)
{
	Entity* ret = nullptr;

	switch (type)
	{
		// L13: Create the corresponding type entity
	case EntityType::PLAYER:
	{
		ret = new Player(x, y);
		break;
	}
	case EntityType::ENEMY:
	{
		ret = new Enemy(x, y, eType);
		break;
	}
	default:
	{
		break;
	}
	}

	// Created entities are added to the list
	if (ret != nullptr) entities.Add(ret);

	return ret;
}

bool EntityManager::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
	{
		app->CapRequest();
	}

	accumulatedTime += dt;
	if (accumulatedTime >= updateMsCycle) doLogic = true;

	UpdateAll(dt, doLogic);

	if (doLogic == true)
	{
		accumulatedTime = 0.0f;
		doLogic = false;
	}

	return true;
}

bool EntityManager::UpdateAll(float dt, bool doLogic)
{
	if (doLogic)
	{
		app->collisions->PreUpdate();
		ListItem<Entity*>* e = entities.start;
		while (e != entities.end)
		{
			e->data->Update(dt);
			if (e->data->currentAnim != nullptr)
			{
				e->data->currentAnim->Update(dt);
			}
			e->data->physics.UpdatePhysics(e->data->nextPos, dt);
			e->data->physics.ResolveCollisions(e->data->collider->rect, e->data->nextPos, e->data->invert);
			if (e->data->collider != nullptr)
			{
				e->data->collider->SetPos(e->data->collider->rect.x, e->data->collider->rect.y, e->data->currentAnim->GetCurrentFrame().w, e->data->currentAnim->GetCurrentFrame().h);
			}
			e->data->PostUpdate(dt);
			e->data->Draw();
			e = e->next;
		}
	}

	return true;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	entities.Del(entities.At(entities.Find(entity)));
}

void EntityManager::OnCollision(Collider* c1, Collider* c2)
{
	for (int i = 0; i < entities.Count(); i++)
	{
		if (c1 == entities[i]->collider)
		{
			entities[i]->OnCollision(c1, c2);
		}
	}
}