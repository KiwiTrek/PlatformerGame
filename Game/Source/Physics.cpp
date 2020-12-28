#include "Physics.h"

#include "App.h"
#include "Collisions.h"
#include "Map.h"

void Physics::UpdatePhysics(iPoint& pos, float dt)
{
	if (axisX)
	{
		pos.x = pos.x + speed.x * dt;
	}
	if (axisY)
	{
		//Euler
		if (verlet == false)
		{
			pos.y = pos.y + speed.y * dt;
		}
		// Verlet
		else
		{
			pos.y = pos.y + speed.y * dt + (gravity * dt * dt * 0.5);
			speed.y = speed.y + gravity * dt;
		}
	}
}

// To know what direction the velocity is going
void Physics::CheckDirection()
{
	if (speed.y >= 0.0f)
	{
		positiveSpeedY = true;
	}
	else if (speed.y < 0.0f)
	{
		positiveSpeedY = false;
	}
}

// Collisions
void Physics::ResolveCollisions(Map* map, SDL_Rect& currentFrame, iPoint nextFrame, bool goingLeft)
{
	iPoint tiledPos(currentFrame.x / GENERAL_TILE_SIZE, currentFrame.y / GENERAL_TILE_SIZE);
	iPoint correctedPos;
	iPoint checkedPos;
	//LOG("past: %d,%d current: %d,%d\n", playerRect.x, playerRect.y, nextFrame.x, nextFrame.y);

	// X axis
	if (!goingLeft) { // right
		tiledPos.x = (currentFrame.x + currentFrame.w) / GENERAL_TILE_SIZE;
		int i = 0;
		while (map->GetTileProperty(tiledPos.x + i, tiledPos.y, "CollisionId") == Collider::Type::AIR && i < 5) {
			i++;
		}
		i--;
		correctedPos.x = MIN(nextFrame.x - currentFrame.x, (tiledPos.x + i) * GENERAL_TILE_SIZE - currentFrame.x);
	}
	else { // left
		int i = 0;
		while (map->GetTileProperty(tiledPos.x - i, tiledPos.y, "CollisionId") == Collider::Type::AIR && i < 5) {
			i++;
		}
		i--;
		correctedPos.x = -MIN(currentFrame.x - nextFrame.x, currentFrame.x - (tiledPos.x - i) * GENERAL_TILE_SIZE);
	}

	// Y axis
	if (positiveSpeedY) {
		tiledPos.y = (currentFrame.y + currentFrame.h) / GENERAL_TILE_SIZE;
		int i = 0;
		while (map->GetTileProperty(tiledPos.x, tiledPos.y + i, "CollisionId") == Collider::Type::AIR && i < 5) {
			i++;
		}
		i--;
		correctedPos.y = MIN(nextFrame.y - currentFrame.y, (tiledPos.y + i) * GENERAL_TILE_SIZE - currentFrame.y);
	}
	else {
		int i = 0;
		while (map->GetTileProperty(tiledPos.x, tiledPos.y - i, "CollisionId") == Collider::Type::AIR && i < 5) {
			i++;
		}
		i--;
		correctedPos.y = -MIN(currentFrame.y - nextFrame.y, currentFrame.y - (tiledPos.y - i) * GENERAL_TILE_SIZE);
	}

	currentFrame.x += correctedPos.x;
	currentFrame.y += correctedPos.y;

	iPoint currentFrameTile = { currentFrame.x / GENERAL_TILE_SIZE, currentFrame.y / GENERAL_TILE_SIZE };

	if (map->GetTileProperty(currentFrameTile.x + 1, currentFrameTile.y, "CollisionId") == Collider::Type::SOLID
		&& map->GetTileProperty(currentFrameTile.x, currentFrameTile.y + 1, "CollisionId") != Collider::Type::SOLID
		&& !goingLeft)
	{
		currentFrame.y -= correctedPos.y;
		speed.x = 0.0f;
		speed.y = 0.0f;
	}
	else if (map->GetTileProperty((currentFrame.x - 1) / GENERAL_TILE_SIZE, currentFrameTile.y, "CollisionId") == Collider::Type::SOLID
		&& map->GetTileProperty(currentFrameTile.x, currentFrameTile.y, "CollisionId") != Collider::Type::SOLID
		&& goingLeft)
	{
		currentFrame.y -= correctedPos.y;
		speed.x = 0.0f;
		speed.y = 0.0f;
	}
	else if (map->GetTileProperty(currentFrameTile.x, currentFrameTile.y + 1, "CollisionId") == Collider::Type::SOLID
		|| map->GetTileProperty(currentFrameTile.x + 1, currentFrameTile.y + 1, "CollisionId") == Collider::Type::SOLID) {
		speed.y = 0.0f;
	}
	else if (map->GetTileProperty(currentFrameTile.x, currentFrameTile.y, "CollisionId") == Collider::Type::SOLID) {
		speed.y = 0.0f;
	}

}