#include "EnemyGround.h"

#include "App.h"
#include "EntityManager.h"
#include "Collisions.h"
#include "PathFinding.h"
#include "Player.h"
#include "Audio.h"
#include "Render.h"

#include "Log.h"

EnemyGround::EnemyGround(int x, int y, EnemyType typeOfEnemy, Entity* playerPointer) : Enemy(x, y, typeOfEnemy, playerPointer)
{
	enemySize = app->generalTileSize;
	for (int i = 0; i != 11; ++i)
	{
		idle.PushBack({ i * enemySize,enemySize,enemySize,enemySize });
	}
	idle.speed = 5.0f;
	idle.loop = true;

	for (int i = 0; i != 8; ++i)
	{
		walking.PushBack({ i * enemySize,enemySize * 2,enemySize,enemySize });
	}
	walking.speed = 5.0f;
	walking.loop = true;

	for (int i = 0; i != 11; ++i)
	{
		hurt.PushBack({ i * enemySize, enemySize * 3, enemySize, enemySize });
	}
	hurt.speed = 30.0f;
	hurt.loop = false;

	for (int i = 0; i != 8; ++i)
	{
		attack.PushBack({ i * enemySize,0, enemySize, enemySize });
	}
	attack.speed = 30.0f;
	attack.loop = false;

	currentAnim = &idle;
	entityRect = { x, y, app->generalTileSize, app->generalTileSize };
	collider = app->collisions->AddCollider(entityRect, Collider::Type::ENEMY, (Module*)app->entities);
	
	idle.Reset();
	walking.Reset();
	hurt.Reset();
	attack.Reset();

	physics.verlet = true;

	invert = true;
}

bool EnemyGround::Update(float dt)
{
	nextPos.x = entityRect.x;
	nextPos.y = entityRect.y;
	physics.speed.x = 0;
	physics.CheckDirection();

	if (attackChange)
	{
		currentAnim = &attack;
	}
	else if (hurtChange)
	{
		currentAnim = &hurt;
	}
	else if (physics.speed.x != 0)
	{
		currentAnim = &walking;
	}

	if (currentAnim->HasFinished() && physics.speed.x == 0)
	{
		if (currentAnim == &hurt)
		{
			pendingToDelete = true;
		}
		else
		{
			if (currentAnim == &attack)
			{
				attackChange = false;
				attack.Reset();
			}
			else if (currentAnim == &walking)
			{
				walking.Reset();
			}
			currentAnim = &idle;
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
	iPoint diffTiles = { abs(destination.x - origin.x), abs(destination.y - origin.y) };
	if (destination.y < 0)
	{
		destination.y = 0;
	}
	if (pastDest != destination)
	{
		pastDest = destination;
		if (diffTiles.x < 10 && diffTiles.y < 10)
		{
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
		else if (path.Count() != 0)
		{
			path.Clear();
		}
	}

	if (path.Count() != 0 && pathCount < 12 && pathCount > 1 && !hurtChange)
	{
		if (i >= (pathCount - 2))
		{
			i = pathCount - 2;
		}

		iPoint pos = app->map->MapToWorld(path.At(i)->x, path.At(i)->y);
		iPoint dest = app->map->MapToWorld(path.At(i+1)->x, path.At(i+1)->y);
		iPoint dif = { dest.x - pos.x,dest.y - pos.y };
		if (dif.x > 0)
		{
			currentAnim = &walking;
			physics.speed.x = 150.0f;
			invert = false;
		}
		else if (dif.x < 0)
		{
			currentAnim = &walking;
			origin.x = (nextPos.x + entityRect.w) / app->generalTileSize;
			physics.speed.x = -75.0f;
			invert = true;
		}

		if (dif.y < 0)
		{
			if (physics.speed.y == 0)
			{
				physics.speed.y = -250.0f;
			}
			physics.positiveSpeedY = false;
		}
		else if (dif.y > 0)
		{
			physics.positiveSpeedY = true;
		}

		counterTile++;
		if (counterTile >= 32)
		{
			counterTile = 32;
		}
		if (counterTile == app->generalTileSize / 2 && physics.speed.y == 0)
		{
			i++;
			counterTile = 0;
		}
	}

	// Call to the base class
	Enemy::Update(dt);

	return true;
}