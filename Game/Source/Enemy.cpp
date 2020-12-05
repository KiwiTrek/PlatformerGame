#include "Enemy.h"
#include "EnemyGround.h"

#include "App.h"
#include "Collisions.h"
#include "Audio.h"
#include "Render.h"
#include "PathFinding.h"

#include "Log.h"

Enemy::Enemy(int x, int y, EnemyType type) : enemyRect({ x, y, 64, 64 }), type(type)
{
	spawnPos.x = enemyRect.x;
	spawnPos.y = enemyRect.y;
	path.Create(DEFAULT_PATH_LENGTH);
}

Enemy::~Enemy()
{
	if (collider != nullptr)
		collider->pendingToDelete = true;
	path.Clear();
}

void Enemy::Update(float dt)
{
	if (currentAnim != nullptr)
		currentAnim->Update(dt);

	enemyPhysics.UpdatePhysics(nextFrame, dt);
	enemyPhysics.ResolveCollisions(enemyRect, nextFrame, invert);

	if (collider != nullptr)
		collider->SetPos(enemyRect.x, enemyRect.y, currentAnim->GetCurrentFrame().w, currentAnim->GetCurrentFrame().h);

	if (app->render->drawAll)
	{
		app->pathfinding->DrawPath(&path);
	}
}

void Enemy::Draw()
{
	if (currentAnim != nullptr)
	{
		app->render->DrawTexture(texture, enemyRect.x, enemyRect.y, false, &(currentAnim->GetCurrentFrame()), invert);
	}

	if (app->render->drawAll)
	{
		app->render->DrawRectangle({ enemyRect.x, enemyRect.y, 64,64 }, 255, 255, 0, 100);
	}
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