#include "Enemy.h"

#include "Audio.h"
#include "Render.h"
#include "PathFinding.h"
#include "Player.h"
#include "Collisions.h"
#include "EntityManager.h"

#include "Log.h"

#define DEFAULT_PATH_LENGTH 50

Enemy::Enemy(int x, int y, EntityManager* listener, EnemyType eType) : Entity(x, y, EntityType::ENEMY, eType)
{
	spawnPos.x = x;
	spawnPos.y = y;
	path.Create(DEFAULT_PATH_LENGTH);
	collider = listener->collisions->AddCollider({ x, y, GENERAL_TILE_SIZE, GENERAL_TILE_SIZE }, Collider::Type::ENEMY, (Module*)listener);
}

Enemy::~Enemy()
{
	if (collider != nullptr)
	{
		collider->pendingToDelete = true;
	}
	path.Clear();
}

void Enemy::OnCollision(Collider* c1, Collider* c2)
{
	if (c2->type == Collider::Type::ATTACK)
	{
		hurtChange = true;
		collider->pendingToDelete = true;
	}
	else
	{
		attackChange = true;
	}
}
