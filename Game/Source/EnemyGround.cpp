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
	collider = app->collisions->AddCollider({ enemyRect.x, enemyRect.y, 64, 64 }, Collider::Type::ENEMY, (Module*)app->enemies);
	
	idle.Reset();
	walking.Reset();
	hurt.Reset();
	attack.Reset();
}

void EnemyGround::Update(float dt)
{
	nextFrame.x = enemyRect.x;
	nextFrame.y = enemyRect.y;
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

	if (currentAnim->HasFinished() || enemyPhysics.speed.x == 0)
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
					enemyPhysics.speed.x = 100.0f;
					invert = false;
				}
				else if (dif.x < 0)
				{
					enemyPhysics.speed.x = -50.0f;
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