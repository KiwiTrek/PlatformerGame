#include "EnemyGround.h"

#include "Enemy.h"
#include "PathFinding.h"
#include "EntityManager.h"
#include "Player.h"
#include "Audio.h"
#include "Map.h"
#include "Render.h"
#include "Collisions.h"

#include "Log.h"

EnemyGround::EnemyGround(int x, int y, EntityManager* listener, EnemyType typeOfEnemy) : Enemy(x, y, listener, typeOfEnemy)
{
	for (int i = 0; i != 11; ++i)
	{
		idle.PushBack({ i * GENERAL_TILE_SIZE,GENERAL_TILE_SIZE,GENERAL_TILE_SIZE,GENERAL_TILE_SIZE });
	}
	idle.speed = 5.0f;
	idle.loop = true;

	for (int i = 0; i != 8; ++i)
	{
		walking.PushBack({ i * GENERAL_TILE_SIZE,GENERAL_TILE_SIZE * 2,GENERAL_TILE_SIZE,GENERAL_TILE_SIZE });
	}
	walking.speed = 5.0f;
	walking.loop = true;

	for (int i = 0; i != 11; ++i)
	{
		hurt.PushBack({ i * GENERAL_TILE_SIZE, GENERAL_TILE_SIZE * 3, GENERAL_TILE_SIZE, GENERAL_TILE_SIZE });
	}
	hurt.speed = 30.0f;
	hurt.loop = false;

	for (int i = 0; i != 8; ++i)
	{
		attack.PushBack({ i * GENERAL_TILE_SIZE,0, GENERAL_TILE_SIZE, GENERAL_TILE_SIZE });
	}
	attack.speed = 30.0f;
	attack.loop = false;

	currentAnim = &idle;

	idle.Reset();
	walking.Reset();
	hurt.Reset();
	attack.Reset();

	invert = true;
}

bool EnemyGround::Update(float dt, Map* map, AudioManager* audio, PathFinding* pathfinding, Entity* player)
{
	nextPos.x = collider->rect.x;
	nextPos.y = collider->rect.y;
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
			active = true;
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

	if (map->GetTileProperty(nextPos.x / GENERAL_TILE_SIZE, nextPos.y / GENERAL_TILE_SIZE + 1, "CollisionId") == Collider::Type::SPIKE)
	{
		hurtChange = true;
		collider->pendingToDelete = true;
		audio->PlayFx(destroyedFx);
	}

	iPoint origin = { nextPos.x / GENERAL_TILE_SIZE,nextPos.y / GENERAL_TILE_SIZE };
	iPoint destination = { player->collider->rect.x / GENERAL_TILE_SIZE,player->collider->rect.y / GENERAL_TILE_SIZE };
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
			pathCount = pathfinding->CreatePath(path, origin, destination);
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

		iPoint pos = map->MapToWorld(path.At(i)->x, path.At(i)->y);
		iPoint dest = map->MapToWorld(path.At(i + 1)->x, path.At(i + 1)->y);
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
			origin.x = (nextPos.x + collider->rect.w) / GENERAL_TILE_SIZE;
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
		if (counterTile == GENERAL_TILE_SIZE / 2 && physics.speed.y == 0)
		{
			i++;
			counterTile = 0;
		}
	}

	// Call to the base class
	return Enemy::Update(dt);
}