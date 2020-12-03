#include "EnemyFlying.h"

#include "App.h"
#include "Collisions.h"
#include "PathFinding.h"
#include "Player.h"

#include "Log.h"

EnemyFlying::EnemyFlying(int x, int y, EnemyType typeOfEnemy) : Enemy(x, y, typeOfEnemy)
{
	for (int i = 0; i != 7; ++i)
	{
		flying.PushBack({ i * enemySize,enemySize,enemySize,enemySize });
	}
	flying.loop = true;

	for (int i = 0; i != 11; ++i)
	{
		hurt.PushBack({ i * enemySize, enemySize * 2, enemySize, enemySize });
	}
	hurt.loop = false;

	for (int i = 0; i != 3; ++i)
	{
		attack.PushBack({ i * enemySize,0, enemySize, enemySize });
	}
	attack.loop = false;

	currentAnim = &flying;
	collider = app->collisions->AddCollider({ enemyRect.x, enemyRect.y, 64, 64 }, Collider::Type::ENEMY, (Module*)app->enemies);

	flying.Reset();
	hurt.Reset();
	attack.Reset();
}

void EnemyFlying::Update(float dt)
{
	if (onceAnim)
	{
		onceAnim = false;
		flying.speed = 1.0f * dt;
		hurt.speed = 2.0f * dt;
		attack.speed = 2.0f * dt;
	}

	nextFrame.x = enemyRect.x;
	nextFrame.y = enemyRect.y;
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

	iPoint origin = { nextFrame.x / 64,nextFrame.y / 64 };
	iPoint destination = { app->player->playerRect.x / 64,app->player->playerRect.y / 64 };
	if (origin.x != destination.x || origin.y != destination.y)
	{
		app->pathfinding->path.Clear();
		if (app->pathfinding->CreatePath(origin, destination) == 0)
		{
			LOG("origin: %d, %d destination: %d, %d\n", origin.x, origin.y, destination.x, destination.y);
		}
		const DynArray<iPoint>* path = app->pathfinding->GetPath();
		iPoint pos = app->map->MapToWorld(path->At(0)->x, path->At(0)->y);
		iPoint dest = app->map->MapToWorld(path->At(1)->x, path->At(1)->y);
		iPoint dif = { dest.x - pos.x,dest.y - pos.y };
		LOG("dif: %d, %d\n", dif.x, dif.y);
		if (dif.x > 0)
		{
			// i do not agree with this
			nextFrame.x += floor(200.0f * dt);
			invert = false;
		}
		else if (dif.x < 0)
		{
			nextFrame.x -= floor(200.0f * dt);
			invert = true;
		}
		else if (dif.y < 0)
		{
			nextFrame.y -= floor(200.0f * dt);
		}
		else if (dif.y > 0)
		{
			nextFrame.y += floor(200.0f * dt);
		}
	}


	// Call to the base class. It must be called at the end
	// It will update the collider depending on the position
	Enemy::Update(dt);
}