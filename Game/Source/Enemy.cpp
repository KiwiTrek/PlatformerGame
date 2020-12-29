#include "Enemy.h"

#include "App.h"
#include "EntityManager.h"
#include "Scene.h"
#include "Collisions.h"
#include "Audio.h"
#include "Render.h"
#include "PathFinding.h"

#include "Log.h"

Enemy::Enemy(int x, int y, EnemyType enemyType, Entity* playerPointer) : Entity(x,y,EntityType::ENEMY, enemyType)
{
	type = enemyType;
	player = playerPointer;
	spawnPos.x = x;
	spawnPos.y = y;
	destroyedFx = app->entities->enemyDestroyedFx;
	physics.axisX = true;
	physics.axisY = true;
	path.Create(DEFAULT_PATH_LENGTH);
}

Enemy::~Enemy()
{
	if (collider != nullptr)
	{
		collider->pendingToDelete = true;
	}
	path.Clear();
}

bool Enemy::Draw()
{
	if (currentAnim != nullptr)
	{
		app->render->DrawTexture(entityTex, collider->rect.x, collider->rect.y, false, &(currentAnim->GetCurrentFrame()), invert);
	}

	if (app->render->drawAll)
	{
		app->render->DrawRectangle({ collider->rect.x, collider->rect.y, app->generalTileSize,app->generalTileSize }, 255, 255, 0, 100);
		app->pathfinding->DrawPath(&path);
	}
	return true;
}

void Enemy::OnCollision(Collider* c1, Collider* c2)
{
	if (c2->type == Collider::Type::ATTACK)
	{
		hurtChange = true;
		collider->pendingToDelete = true;
		app->audio->PlayFx(destroyedFx);
		app->scene->scoreValue += 100;
	}
	else
	{
		attackChange = true;
	}
}