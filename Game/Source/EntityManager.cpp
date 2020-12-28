#include "EntityManager.h"

#include "Player.h"
#include "Enemy.h"
#include "Collisions.h"
#include "Map.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "App.h"

#include "Defs.h"
#include "Log.h"

EntityManager::EntityManager(Render* renderM, Textures* textures,Input* inputM, Map* mapM, AudioManager* audioM, App* appM) : Module()
{
	name.Create("entitymanager");
	render = renderM;
	tex = textures;
	map = mapM;
	audio = audioM;
	input = inputM;
	app = appM;
	collisions = new Collisions();
}

// Destructor
EntityManager::~EntityManager()
{}

void EntityManager::Init()
{
	active = false;
}

// Called before render is available
bool EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Loading Entity Manager");
	bool ret = true;

	folderAudioFx.Create(config.child("folderAudioFx").child_value());
	folderTexture.Create(config.child("folderTexture").child_value());

	// Fx
	SString tmp("%s%s", folderAudioFx.GetString(), "enemy_death.wav");
	enemyDestroyedFx = audio->LoadFx(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "lose.wav");
	deadFx = audio->LoadFx(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "jump.wav");
	jumpFx = audio->LoadFx(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "double_jump.wav");
	doubleJumpFx = audio->LoadFx(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "fruit.wav");
	fruitFx = audio->LoadFx(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "hit.wav");
	hitFx = audio->LoadFx(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "slash.wav");
	slashFx = audio->LoadFx(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "checkpoint.wav");
	checkpointFx = audio->LoadFx(tmp.GetString());

	// Tex
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "character_spritesheet.png");
	playerTex = tex->Load(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "heart.png");
	playerHeart = tex->Load(tmp.GetString());
	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	if (!active) return true;

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
		ret = new Player(x, y, this);
		break;
	}
	case EntityType::ENEMY:
	{
		ret = new Enemy(x, y, this, eType);
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
	ListItem<Entity*>* e = entities.start;
	while (e != entities.end)
	{
		if (e->data->type == EntityType::PLAYER)
		{
			if (input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN && e->data->IsIdle())
			{
				app->SaveRequest();
			}
			if (input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN && e->data->IsIdle())
			{
				app->LoadRequest();
			}
		}
		e = e->next;
	}

	if (input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
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
		collisions->PreUpdate();
		ListItem<Entity*>* e = entities.start;
		while (e != entities.end)
		{
			e->data->Update(dt);
			if (e->data->currentAnim != nullptr)
			{
				e->data->currentAnim->Update(dt);
			}
			e->data->physics.UpdatePhysics(e->data->nextPos, dt);
			e->data->physics.ResolveCollisions(map, e->data->collider->rect, e->data->nextPos, e->data->invert);
			if (e->data->collider != nullptr)
			{
				e->data->collider->SetPos(e->data->collider->rect.x, e->data->collider->rect.y, e->data->currentAnim->GetCurrentFrame().w, e->data->currentAnim->GetCurrentFrame().h);
			}
			if (e->data->type == EntityType::ENEMY)
			{
				map->DrawPath(render, &e->data->path);
			}
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
			switch (c1->type)
			{
			case Collider::Type::ENEMY:
				if (c2->type == Collider::Type::ATTACK)
				{
					LOG("Player collision!\n");
					audio->PlayFx(enemyDestroyedFx);
				}
				break;
			case Collider::Type::PLAYER:
				if (c2->type == Collider::Type::ENEMY)
				{
					LOG("Enemy collision!\n");
					audio->PlayFx(hitFx);
				}
				break;
			default:
				break;
			}
		}
	}
}