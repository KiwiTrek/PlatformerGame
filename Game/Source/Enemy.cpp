#include "Enemy.h"

#include "App.h"
#include "Collisions.h"
#include "Audio.h"
#include "Render.h"

Enemy::Enemy(int x, int y) : position(x, y)
{
	spawnPos = position;
}

Enemy::~Enemy()
{
	if (collider != nullptr)
		collider->pendingToDelete = true;
}

const Collider* Enemy::GetCollider() const
{
	return collider;
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

//void Enemy::OnCollision(Collider* collider)
//{
//}