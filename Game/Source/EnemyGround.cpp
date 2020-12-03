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

	for (int i = 0; i != 11; ++i)
	{
		hurt.PushBack({ i * enemySize, enemySize * 3, enemySize, enemySize });
	}
	hurt.loop = false;
	hurt.speed = 0.3f;

	for (int i = 0; i != 8; ++i)
	{
		attack.PushBack({ i * enemySize,0, enemySize, enemySize });
	}
	attack.loop = false;
	attack.speed = 0.3f;

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
	if (hurtChange)
	{
		currentAnim = &hurt;
	}

	if (currentAnim->HasFinished())
	{
		if (currentAnim == &attack)
		{
			currentAnim = &idle;
			attackChange = false;
			attack.Reset();
		}
		else if (currentAnim == &hurt)
		{
			pendingToDelete = true;
		}
	}

	// Test speed
	enemyPhysics.speed.x = 200.0f;
	
	// Call to the base class. It must be called at the end
	// It will update the collider depending on the position
	Enemy::Update(dt);
}