#include "Enemy.h"
#include "EnemyGround.h"

#include "App.h"
#include "Collisions.h"
#include "Audio.h"
#include "Render.h"

#include "Log.h"

Enemy::Enemy(int x, int y, EnemyType type) : position(x, y), type(type)
{
	spawnPos = position;
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

	if (collider != nullptr)
		collider->SetPos(position.x, position.y,currentAnim->GetCurrentFrame().w, currentAnim->GetCurrentFrame().h);
}

void Enemy::Draw()
{
	if (currentAnim != nullptr)
		app->render->DrawTexture(texture, position.x, position.y, false, &(currentAnim->GetCurrentFrame()));
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