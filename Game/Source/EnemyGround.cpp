#include "EnemyGround.h"

#include "App.h"
#include "Collisions.h"

#include "Log.h"

EnemyGround::EnemyGround(int x, int y, EnemyType typeOfEnemy) : Enemy(x, y, typeOfEnemy)
{
	fly.PushBack({ 5,72,21,22 });
	currentAnim = &fly;

	collider = app->collisions->AddCollider({ 0, 0, 24, 24 }, Collider::Type::ENEMY, (Module*)app->enemies);
}

void EnemyGround::Update(float dt)
{
	position = spawnPos;

	// Call to the base class. It must be called at the end
	// It will update the collider depending on the position
	Enemy::Update(dt);
}

void EnemyGround::OnCollision(Collider* c1, Collider* c2)
{
	if (type == EnemyType::GROUND)
	{
		LOG("Ground Enemy - Player collision!\n");
	}
	else if (type == EnemyType::FLYING)
	{
		LOG("Flying Enemy - Player collision!\n");
	}
}