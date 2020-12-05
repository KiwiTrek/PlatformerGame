#include "EnemyManagement.h"

#include "App.h"

#include "Window.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Collisions.h"

#include "Enemy.h"
#include "EnemyGround.h"
#include "EnemyFlying.h"

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
	flying = app->tex->Load("Assets/Textures/enemy_flying_spritesheet.png");
	enemyGroundFx = app->audio->LoadFx("Assets/Audio/Fx/ground_chasing.wav");
	enemyFlyingFx = app->audio->LoadFx("Assets/Audio/Fx/flying_chasing.wav");
	enemyDestroyedFx = app->audio->LoadFx("Assets/Audio/Fx/enemy_death.wav");

	return true;
}

bool EnemyManagement::Update(float dt)
{
	HandleEnemiesSpawn();

	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr)
		{
			enemies[i]->Update(dt);
		}
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

bool EnemyManagement::Load(pugi::xml_node& save)
{
	LOG("Loading enemy data");
	bool ret = true;
	int i = 0;

	int x = 0;
	int y = 0;
	EnemyType type = EnemyType::NO_TYPE;
	bool destroyed = false;

	for (pugi::xml_node enemy = save.child("enemy"); enemy && ret; enemy = enemy.next_sibling("enemy"))
	{
		destroyed = enemy.child("destroyed").attribute("value").as_bool();
		if (destroyed == true && enemies[i] != nullptr)
		{
			enemies[i]->pendingToDelete = true;
		}
		else
		{
			x = enemy.child("coordinates").attribute("x").as_int();
			y = enemy.child("coordinates").attribute("y").as_int();
			switch (enemy.child("type").attribute("value").as_int())
			{
			case 1:
				type = EnemyType::GROUND;
				break;
			case 2:
				type = EnemyType::FLYING;
				break;
			default:
				type = EnemyType::NO_TYPE;
				break;
			}

			if (enemies[i] == nullptr)
			{
				AddEnemy(type, x, y, i);
			}
			else
			{
				enemies[i]->enemyRect.x = x;
				enemies[i]->enemyRect.y = y;
			}
		}
		i++;
	}

	return ret;
}

bool EnemyManagement::Save(pugi::xml_node& save)
{
	LOG("Saving enemy data");
	bool ret = true;

	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		pugi::xml_node enemy = save.append_child("enemy");
		if (enemies[i] != nullptr)
		{
			pugi::xml_node enemyCoords = enemy.append_child("coordinates");
			enemyCoords.append_attribute("x").set_value(enemies[i]->enemyRect.x);
			enemyCoords.append_attribute("y").set_value(enemies[i]->enemyRect.y);
			enemy.append_child("type").append_attribute("value").set_value(enemies[i]->type);
			enemy.append_child("destroyed").append_attribute("value").set_value(enemies[i]->pendingToDelete);
		}
		else
		{
			enemy.append_child("destroyed").append_attribute("value").set_value(true);
		}
	}
	return ret;
}

bool EnemyManagement::AddEnemy(EnemyType type, int x, int y, uint i)
{
	bool ret = false;
	if (i == -1)
	{
		for (i = 0; i < MAX_ENEMIES; ++i)
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
	}
	else
	{
		if (spawnQueue[i].type == EnemyType::NO_TYPE)
		{
			spawnQueue[i].type = type;
			spawnQueue[i].x = x;
			spawnQueue[i].y = y;
			ret = true;
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
			if (enemies[i]->pendingToDelete)
			{
				LOG("DeSpawning enemy at %d", enemies[i]->enemyRect.x * app->win->GetScale());

				delete enemies[i];
				enemies[i] = nullptr;
			}
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
				enemies[i] = new EnemyGround(info.x, info.y, info.type);
				enemies[i]->texture = ground;
				enemies[i]->chasingFx = enemyGroundFx;
				enemies[i]->enemyPhysics.axisY = true;
				break;
			case EnemyType::FLYING:
				enemies[i] = new EnemyFlying(info.x, info.y, info.type);
				enemies[i]->texture = flying;
				enemies[i]->chasingFx = enemyFlyingFx;
				enemies[i]->enemyPhysics.axisY = false;
				break;
			}
			enemies[i]->enemyPhysics.axisX = true;
			enemies[i]->destroyedFx = enemyDestroyedFx;
			app->audio->SetFxVolume(enemies[i]->destroyedFx);
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