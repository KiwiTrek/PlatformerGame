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
	collider = app->collisions->AddCollider({ enemyRect.x, enemyRect.y, 64, 64 }, Collider::Type::ENEMY, (Module*)app->enemies);

	flying.Reset();
	hurt.Reset();
	attack.Reset();
}

void EnemyFlying::Update(float dt)
{
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
	if (pastDest != destination)
	{
		pastDest = destination;
		if (origin.x != destination.x || origin.y != destination.y)
		{
			if (app->map->GetTileProperty(nextFrame.x / 64, nextFrame.y / 64 + 1, "CollisionId") == Collider::Type::SPIKE)
			{
				hurtChange = true;
				collider->pendingToDelete = true;
				app->audio->PlayFx(destroyedFx);
			}
			else
			{
				app->pathfinding->path.Clear();
				i = 0;
				j = 1;
				if (app->pathfinding->CreatePath(origin, destination) == 0)
				{
					LOG("origin: %d, %d destination: %d, %d\n", origin.x, origin.y, destination.x, destination.y);
				}
				currentPath = *app->pathfinding->GetPath();
			}

			if (currentPath.Count() < 12)
			{
				i++;
				j++;
				if (i > currentPath.Count())
				{
					i = currentPath.Count();
				}
				if (j > currentPath.Count())
				{
					j = currentPath.Count();
				}
				iPoint pos = app->map->MapToWorld(currentPath.At(i)->x, currentPath.At(i)->y);
				iPoint dest = app->map->MapToWorld(currentPath.At(j)->x, currentPath.At(j)->y);
				iPoint dif = { dest.x - pos.x,dest.y - pos.y };
				LOG("dif: %d, %d\n", dif.x, dif.y);
				if (dif.x > 0)
				{
					// i do not agree with this
					enemyPhysics.speed.x = 50.0f;
					invert = false;
				}
				else if (dif.x < 0)
				{
					enemyPhysics.speed.x = -25.0f;
					invert = true;
				}
				else if (dif.y < 0)
				{
					nextFrame.y -= floor(150.0f * dt);
				}
				else if (dif.y > 0)
				{
					nextFrame.y += floor(150.0f * dt);
				}
			}
		}
	}

	if (app->render->drawAll)
	{
		app->pathfinding->DrawPath(&currentPath);
	}

	// Call to the base class. It must be called at the end
	// It will update the collider depending on the position
	Enemy::Update(dt);
}