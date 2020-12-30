#include "EnemyFlying.h"

#include "App.h"
#include "EntityManager.h"
#include "Collisions.h"
#include "PathFinding.h"
#include "Player.h"
#include "Audio.h"
#include "Render.h"

#include "Log.h"

EnemyFlying::EnemyFlying(int x, int y, EnemyType typeOfEnemy, Entity* playerPointer) : Enemy(x, y, typeOfEnemy, playerPointer)
{
	enemySize = app->generalTileSize;
	for (int i = 0; i != 7; ++i)
	{
		flying.PushBack({ i * enemySize,enemySize,enemySize,enemySize });
	}
	flying.speed = 10.0f;
	flying.loop = true;

	for (int i = 0; i != 11; ++i)
	{
		hurt.PushBack({ i * enemySize, enemySize * 2, enemySize, enemySize });
	}
	hurt.speed = 20.0f;
	hurt.loop = false;

	for (int i = 0; i != 3; ++i)
	{
		attack.PushBack({ i * enemySize,0, enemySize, enemySize });
	}
	attack.speed = 20.0f;
	attack.loop = false;

	currentAnim = &flying;
	entityRect = { x, y, app->generalTileSize, app->generalTileSize };
	collider = app->collisions->AddCollider(entityRect, Collider::Type::ENEMY, (Module*)app->entities);

	flying.Reset();
	hurt.Reset();
	attack.Reset();

	entityTex = app->entities->flying;
	physics.verlet = false;

	invert = true;
}

bool EnemyFlying::Update(float dt)
{
	nextPos.x = entityRect.x;
	nextPos.y = entityRect.y;
	physics.speed.x = 0;
	physics.speed.y = 0;
	physics.CheckDirection();

	if (attackChange)
	{
		currentAnim = &attack;
	}
	if (hurtChange)
	{
		currentAnim = &hurt;
	}

	if (currentAnim->HasFinished())
	{
		if (currentAnim == &attack)
		{
			currentAnim = &flying;
			attackChange = false;
			attack.Reset();
		}
		else if (currentAnim == &hurt)
		{
			pendingToDelete = true;
		}
	}

	if (app->map->GetTileProperty(nextPos.x / app->generalTileSize, nextPos.y / app->generalTileSize + 1, "CollisionId") == Collider::Type::SPIKE)
	{
		hurtChange = true;
		collider->pendingToDelete = true;
		app->audio->PlayFx(destroyedFx);
	}

	iPoint origin = { nextPos.x / app->generalTileSize,nextPos.y / app->generalTileSize };
	iPoint destination = { player->entityRect.x / app->generalTileSize,player->entityRect.y / app->generalTileSize };
	if (destination.y < 0)
	{
		destination.y = 0;
	}
	if (pastDest != destination)
	{
		pastDest = destination;
		if (origin.x != destination.x || origin.y != destination.y)
		{
			path.Clear();
			pathCount = app->pathfinding->CreatePath(path, origin, destination);
			if (pathCount != -1)
			{
				i = 0;
			}
		}
	}
	
	if (pathCount < 12 && pathCount > 1 && !hurtChange)
	{
		if (i >= (pathCount - 2))
		{
			i = pathCount - 2;
		}

		iPoint pos = app->map->MapToWorld(path.At(i)->x, path.At(i)->y);
		iPoint dest = app->map->MapToWorld(path.At(i + 1)->x, path.At(i + 1)->y);
		iPoint dif = { dest.x - pos.x,dest.y - pos.y };
		if (dif.x > 0)
		{
			physics.speed.x = 150.0f;
			invert = false;
		}
		else if (dif.x < 0)
		{
			origin.x = (nextPos.x + entityRect.w) / app->generalTileSize;
			physics.speed.x = -75.0f;
			invert = true;
		}
		else if (dif.y < 0)
		{
			origin.y = (nextPos.y + entityRect.h) / app->generalTileSize;
			physics.speed.y = -75.0f;
		}
		else if (dif.y > 0)
		{
			physics.speed.y = 150.0f;
		}
	}

	counterTile++;
	if (counterTile == app->generalTileSize/2)
	{
		i++;
		counterTile = 0;
	}

	// Call to the base class
	Enemy::Update(dt);

	return true;
}