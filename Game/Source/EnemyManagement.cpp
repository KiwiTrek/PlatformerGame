#include "EnemyManagement.h"

#include "App.h"

#include "Window.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Collisions.h"

#include "Enemy.h"
#include "EnemyGround.h"

#include "Defs.h"
#include "Log.h"

#define SPAWN_MARGIN 50


EnemyManagement::EnemyManagement()
{
	for (uint i = 0; i < MAX_ENEMIES; ++i)
		enemies[i] = nullptr;

	name.Create("enemyManagement");
}

EnemyManagement::~EnemyManagement()
{

}

void EnemyManagement::Init()
{
	active = false;
}

bool EnemyManagement::Awake(pugi::xml_node& config)
{
	return true;
}

bool EnemyManagement::Start()
{
	ground = app->tex->Load("Assets/Textures/enemy_ground_spritesheet.png");
	death = app->tex->Load("Assets/Textures/smoke.png");
	enemyGroundFx = app->audio->LoadFx("Assets/Audio/Fx/ground_idle.wav");
	enemyDestroyedFx = app->audio->LoadFx("Assets/Audio/Fx/enemy_death.wav");

	return true;
}

bool EnemyManagement::Update(float dt)
{
	HandleEnemiesSpawn();

	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr)
			enemies[i]->Update(dt);
	}

	HandleEnemiesDespawn();

	return true;
}

bool EnemyManagement::PostUpdate()
{
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr)
			enemies[i]->Draw();
	}

	return true;
}

// Called before quitting
bool EnemyManagement::CleanUp()
{
	LOG("Freeing all enemies");

	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr)
		{
			delete enemies[i];
			enemies[i] = nullptr;
		}
	}

	return true;
}

bool EnemyManagement::AddEnemy(EnemyType type, int x, int y)
{
	bool ret = false;

	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (spawnQueue[i].type == EnemyType::NO_TYPE)
		{
			spawnQueue[i].type = type;
			spawnQueue[i].x = x;
			spawnQueue[i].y = y;
			ret = true;
			break;
		}
	}

	return ret;
}

void EnemyManagement::HandleEnemiesSpawn()
{
	// Iterate all the enemies queue
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (spawnQueue[i].type != EnemyType::NO_TYPE)
		{
			LOG("Spawning enemy at %d", spawnQueue[i].x * app->win->GetScale());
			SpawnEnemy(spawnQueue[i]);
			spawnQueue[i].type = EnemyType::NO_TYPE; // Removing the newly spawned enemy from the queue
		}
	}
}

void EnemyManagement::HandleEnemiesDespawn()
{
	// Iterate existing enemies
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr)
		{
			// Delete the enemy when it has reached the end of the screen
			/*if (enemies[i]->position.x * app->win->GetScale() < (app->render->camera.x) - SPAWN_MARGIN)
			{
				LOG("DeSpawning enemy at %d", enemies[i]->position.x * app->win->GetScale());

				delete enemies[i];
				enemies[i] = nullptr;
			}*/
		}
	}
}

void EnemyManagement::SpawnEnemy(const EnemySpawnpoint& info)
{
	// Find an empty slot in the enemies array
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] == nullptr)
		{
			switch (info.type)
			{
			case EnemyType::GROUND:
				//enemies[i] = new Enemy_RedBird(info.x, info.y);
				enemies[i] = new EnemyGround(info.x, info.y, info.type);
				enemies[i]->texture = ground;
				enemies[i]->chasingFx = enemyGroundFx;
				break;
			case EnemyType::FLYING:
				//enemies[i] = new Enemy_BrownShip(info.x, info.y);
				//enemies[i]->texture = flying;
				//enemies[i]->chasingFx = enemyFlyingFx;
				break;
			}
			enemies[i]->deathTexture = death;
			enemies[i]->destroyedFx = enemyDestroyedFx;
			break;
		}
	}
}

void EnemyManagement::OnCollision(Collider* c1, Collider* c2)
{
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr && enemies[i]->collider == c1)
		{
			enemies[i]->OnCollision(c1, c2); //Notify the enemy of a collision
		}
	}
}