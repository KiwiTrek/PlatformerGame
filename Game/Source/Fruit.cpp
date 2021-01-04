#include "Fruit.h"

#include "App.h"

#include "Audio.h"
#include "Render.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Animation.h"

#include "Log.h"

Fruit::Fruit(int x, int y) : Entity(x, y, EntityType::FRUIT)
{
	pendingToDelete = false;
	entityRect = { x, y, app->generalTileSize, app->generalTileSize };
	collider = app->collisions->AddCollider(entityRect, Collider::Type::FRUIT, (Module*)app->entities);
	fruitSect = { 383,192,app->generalTileSize,app->generalTileSize };
	//447 - 383;
	//256 - 192;
	invert = false;

	physics.axisX = false;
	physics.axisY = false;
	physics.positiveSpeedY = false;
	physics.verlet = false;

	// Fx
	fruitFx = app->entities->fruitFx;

	// Textures
	entityTex = app->entities->fruit;
}

bool Fruit::Draw()
{
	app->render->DrawTexture(entityTex, entityRect.x, entityRect.y, false, &fruitSect, invert);

	if (app->render->drawAll)
	{
		app->render->DrawRectangle({ entityRect.x, entityRect.y, app->generalTileSize, app->generalTileSize }, 255, 255, 0, 100);
	}
	return true;
}

void Fruit::OnCollision(Collider* c1, Collider* c2)
{
	app->scene->fruitCounter++;
	app->scene->scoreValue += 100;
	app->audio->PlayFx(fruitFx);
	pendingToDelete = true;
	this->collider->pendingToDelete = true;
}