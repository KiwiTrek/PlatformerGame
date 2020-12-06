#include "EnemyFlying.h"

#include "App.h"
#include "Collisions.h"
#include "PathFinding.h"
#include "Player.h"
#include "Audio.h"
#include "Render.h"

#include "Log.h"

EnemyFlying::EnemyFlying(int x, int y, EnemyType typeOfEnemy) : Enemy(x, y, typeOfEnemy)
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
	collider = app->collisions->AddCollider({ enemyRect.x, enemyRect.y, app->generalTileSize, app->generalTileSize }, Collider::Type::ENEMY, (Module*)app->enemies);

	flying.Reset();
	hurt.Reset();
	attack.Reset();
}

void EnemyFlying::Update(float dt)
{
	nextFrame.x = enemyRect.x;
	nextFrame.y = enemyRect.y;
	enemyPhysics.speed.x = 0;
	enemyPhysics.CheckDirection();

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

	if (app->map->GetTileProperty(nextFrame.x / app->generalTileSize, nextFrame.y / app->generalTileSize + 1, "CollisionId") == Collider::Type::SPIKE)
	{
		hurtChange = true;
		collider->pendingToDelete = true;
		app->audio->PlayFx(destroyedFx);
	}

	iPoint origin = { nextFrame.x / app->generalTileSize,nextFrame.y / app->generalTileSize };
	iPoint destination = { app->player->playerRect.x / app->generalTileSize,app->player->playerRect.y / app->generalTileSize };
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
				//LOG("origin: %d, %d destination: %d, %d\n", origin.x, origin.y, destination.x, destination.y);
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
		//LOG("dif: %d, %d\n", dif.x, dif.y);
		if (dif.x > 0)
		{
			// i do not agree with this
			enemyPhysics.speed.x = 150.0f;
			//if (origin.x <= dest.x)
			//{
			//	i++;
			//}
			invert = false;
		}
		else if (dif.x < 0)
		{
			origin.x = (nextFrame.x + enemyRect.w) / app->generalTileSize;
			enemyPhysics.speed.x = -75.0f;
			invert = true;
			//if (origin.x >= dest.x)
			//{
			//	i++;
			//}
		}
		else if (dif.y < 0)
		{
			origin.y = (nextFrame.y + enemyRect.h) / app->generalTileSize;
			enemyPhysics.speed.y = -75.0f;
			//if (origin.y >= dest.y)
			//{
			//	i++;
			//}
		}
		else if (dif.y > 0)
		{
			enemyPhysics.speed.y = 150.0f;
			//if (origin.y <= dest.y)
			//{
			//	i++;
			//}
		}
	}

	// Call to the base class. It must be called at the end
	// It will update the collider depending on the position
	Enemy::Update(dt);
}