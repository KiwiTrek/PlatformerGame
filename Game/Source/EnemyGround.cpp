#include "EnemyGround.h"

#include "App.h"
#include "Collisions.h"

#include "Log.h"

EnemyGround::EnemyGround(int x, int y, EnemyType typeOfEnemy) : Enemy(x, y, typeOfEnemy)
{
	for (int i = 0; i != 11; ++i)
	{
		idle.PushBack({ i * enemySize,enemySize,enemySize,enemySize });
	}
	idle.loop = true;
	idle.speed = 0.1f;

	for (int i = 0; i != 8; ++i)
	{
		walking.PushBack({ i * enemySize,enemySize * 2,enemySize,enemySize });
	}
	walking.loop = true;
	walking.speed = 0.1f;

	for (int i = 0; i != 3; ++i)
	{
		hurt.PushBack({ i * enemySize, enemySize * 3, enemySize, enemySize });
	}
	hurt.loop = false;
	hurt.speed = 0.1f;

	currentAnim = &idle;
	collider = app->collisions->AddCollider({ position.x, position.y, 64, 64 }, Collider::Type::ENEMY, (Module*)app->enemies);
}

void EnemyGround::Update(float dt)
{
	// Call to the base class. It must be called at the end
	// It will update the collider depending on the position
	Enemy::Update(dt);
}