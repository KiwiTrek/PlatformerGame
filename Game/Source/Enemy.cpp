#include "Enemy.h"
#include "EnemyGround.h"

#include "App.h"
#include "Collisions.h"
#include "Audio.h"
#include "Render.h"

#include "Log.h"

Enemy::Enemy(int x, int y, EnemyType type) : enemyRect({ x, y, 64, 64 }), type(type)
{
	spawnPos.x = enemyRect.x;
	spawnPos.y = enemyRect.y;
	LOG("Enemy Rect Origin: %d %d", enemyRect.x, enemyRect.y);
}

Enemy::~Enemy()
{
	if (collider != nullptr)
		collider->pendingToDelete = true;
}

void Enemy::Update(float dt)
{
	if (currentAnim != nullptr)
		currentAnim->Update();

	enemyPhysics.UpdatePhysics(nextFrame, dt);
	enemyPhysics.ResolveCollisions(enemyRect, nextFrame, invert);
	if (type == EnemyType::GROUND)
	{
		LOG("Position = %d %d", enemyRect.x, enemyRect.y);
	}

	if (collider != nullptr)
		collider->SetPos(enemyRect.x, enemyRect.y, currentAnim->GetCurrentFrame().w, currentAnim->GetCurrentFrame().h);

}

void Enemy::Draw()
{
	if (currentAnim != nullptr)
		app->render->DrawTexture(texture, enemyRect.x, enemyRect.y, false, &(currentAnim->GetCurrentFrame()));
}

void Enemy::OnCollision(Collider* c1, Collider* c2)
{
	if (c2->type == Collider::Type::ATTACK)
	{
		hurtChange = true;
		collider->pendingToDelete = true;
		app->audio->PlayFx(destroyedFx);
	}
	else
	{
		attackChange = true;
	}
}