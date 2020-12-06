#include "EnemyGround.h"

#include "App.h"
#include "Collisions.h"
#include "PathFinding.h"
#include "Player.h"
#include "Audio.h"
#include "Render.h"

#include "Log.h"

EnemyGround::EnemyGround(int x, int y, EnemyType typeOfEnemy) : Enemy(x, y, typeOfEnemy)
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
	collider = app->collisions->AddCollider({ enemyRect.x, enemyRect.y, app->generalTileSize, app->generalTileSize }, Collider::Type::ENEMY, (Module*)app->enemies);
	
	idle.Reset();
	walking.Reset();
	hurt.Reset();
	attack.Reset();
}

void EnemyGround::Update(float dt)
{
	nextFrame.x = enemyRect.x;
	nextFrame.y = enemyRect.y;
	enemyPhysics.speed.x = 0;
	enemyPhysics.CheckDirection();

	if (attackChange)
	{
		currentAnim = &attack;
	}
	else if (hurtChange)
	{
		currentAnim = &hurt;
	}
	else if (enemyPhysics.speed.x != 0)
	{
		currentAnim = &walking;
	}

	if (currentAnim->HasFinished() && enemyPhysics.speed.x == 0)
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
		iPoint dest = app->map->MapToWorld(path.At(i+1)->x, path.At(i+1)->y);
		iPoint dif = { dest.x - pos.x,dest.y - pos.y };
		//LOG("dif: %d, %d\n", dif.x, dif.y);
		if (dif.x > 0)
		{
			// i do not agree with this
			currentAnim = &walking;
			enemyPhysics.speed.x = 150.0f;
			invert = false;
		}
		else if (dif.x < 0)
		{
			currentAnim = &walking;
			origin.x = (nextFrame.x + enemyRect.w) / app->generalTileSize;
			enemyPhysics.speed.x = -75.0f;
			invert = true;
		}

		if (dif.y < 0)
		{
			if (enemyPhysics.speed.y == 0)
			{
				enemyPhysics.speed.y = -250.0f;
			}
			enemyPhysics.positiveSpeedY = false;
		}
		else if (dif.y > 0)
		{
			enemyPhysics.positiveSpeedY = true;
		}
	}

	// Call to the base class. It must be called at the end
	// It will update the collider depending on the position
	Enemy::Update(dt);
}