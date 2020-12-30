#include "EntityManager.h"

#include "App.h"

#include "Collisions.h"
#include "Map.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "PathFinding.h"

#include "Player.h"
//#include "Enemy.h"
#include "EnemyFlying.h"
#include "EnemyGround.h"

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
	ListItem<Entity*>* e = entities.start;
	while (e != nullptr)
	{
		ListItem<Entity*>* eNext = e->next;
		DestroyEntity(e->data);
		e = eNext;
	}

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

Entity* EntityManager::CreateEntity(int x, int y, EntityType type, Entity* playerPointer, EnemyType eType)
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
		switch (eType)
		{
		case EnemyType::FLYING:
		{
			ret = new EnemyFlying(x, y, eType, playerPointer);
			break;
		}
		case EnemyType::GROUND:
		{
			ret = new EnemyGround(x, y, eType, playerPointer);
			break;
		}
		default:
		{
			break;
		}
		}
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
		while (e != nullptr)
		{
			e->data->Update(dt);
			if (e->data->currentAnim != nullptr)
			{
				e->data->currentAnim->Update(dt);
			}
			e->data->physics.UpdatePhysics(e->data->nextPos, dt);
			e->data->physics.ResolveCollisions(e->data->entityRect, e->data->nextPos, e->data->invert);
			if (e->data->collider != nullptr)
			{
				e->data->collider->SetPos(e->data->entityRect.x, e->data->entityRect.y, e->data->currentAnim->GetCurrentFrame().w, e->data->currentAnim->GetCurrentFrame().h);
			}
			e->data->PostUpdate(dt);
			e = e->next;
		}
	}

	return true;
}

bool EntityManager::PostUpdate()
{
	ListItem<Entity*>* e = entities.start;
	while (e != nullptr)
	{
		if (e->data->currentAnim != nullptr)
		{
			e->data->Draw();
		}
		if (e->data->pendingToDelete == true)
		{
			DestroyEntity(e->data);
		}
		e = e->next;
	}
	return true;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	entity->collider->pendingToDelete = true;
	int i = entities.Find(entity);
	delete entities[i];
	entities.Del(entities.At(i));
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

//bool EnemyManagement::Load(pugi::xml_node& save)
//{
//	LOG("Loading enemy data");
//	bool ret = true;
//	int i = 0;
//
//	int x = 0;
//	int y = 0;
//	EnemyType type = EnemyType::NO_TYPE;
//	bool destroyed = false;
//
//	for (pugi::xml_node enemy = save.child("enemy"); enemy && ret; enemy = enemy.next_sibling("enemy"))
//	{
//		destroyed = enemy.child("destroyed").attribute("value").as_bool();
//		if (destroyed == true && enemies[i] != nullptr)
//		{
//			enemies[i]->pendingToDelete = true;
//		}
//		else
//		{
//			x = enemy.child("coordinates").attribute("x").as_int();
//			y = enemy.child("coordinates").attribute("y").as_int();
//			switch (enemy.child("type").attribute("value").as_int())
//			{
//			case 1:
//				type = EnemyType::GROUND;
//				break;
//			case 2:
//				type = EnemyType::FLYING;
//				break;
//			default:
//				type = EnemyType::NO_TYPE;
//				break;
//			}
//
//			if (enemies[i] == nullptr)
//			{
//				AddEnemy(type, x, y, i);
//			}
//			else
//			{
//				enemies[i]->enemyRect.x = x;
//				enemies[i]->enemyRect.y = y;
//			}
//		}
//		i++;
//	}
//
//	return ret;
//}
//
//bool EnemyManagement::Save(pugi::xml_node& save)
//{
//	LOG("Saving enemy data");
//	bool ret = true;
//
//	for (uint i = 0; i < MAX_ENEMIES; ++i)
//	{
//		pugi::xml_node enemy = save.append_child("enemy");
//		if (enemies[i] != nullptr)
//		{
//			pugi::xml_node enemyCoords = enemy.append_child("coordinates");
//			enemyCoords.append_attribute("x").set_value(enemies[i]->enemyRect.x);
//			enemyCoords.append_attribute("y").set_value(enemies[i]->enemyRect.y);
//			enemy.append_child("type").append_attribute("value").set_value(enemies[i]->type);
//			enemy.append_child("destroyed").append_attribute("value").set_value(enemies[i]->pendingToDelete);
//		}
//		else
//		{
//			enemy.append_child("destroyed").append_attribute("value").set_value(true);
//		}
//	}
//	return ret;
//}