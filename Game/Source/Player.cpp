#include "Player.h"

#include "App.h"

#include "Textures.h"
#include "Input.h"
#include "Audio.h"
#include "Render.h"
#include "Scene.h"
#include "DeathScene.h"
#include "Transition.h"
#include "Map.h"
#include "Collisions.h"
#include "EntityManager.h"

#include "Log.h"

Player::Player(int x, int y) : Entity(x, y, EntityType::PLAYER)
{
	spawnPos = GetSpawnPoint();
	pendingToDelete = false;
	entityRect = { spawnPos.x, spawnPos.y, app->generalTileSize, app->generalTileSize };
	collider = app->collisions->AddCollider(entityRect, Collider::Type::PLAYER, (Module*)app->entities);

	playerSize = 128;
	jumpCounter = 2;
	lives = 3;
	hitCD = 0;

	keyPressed = false;
	godMode = false;
	isDead = false;
	isJumping = false;
	isHit = false;
	isAttacking = false;
	invert = false;
	debugDraw = false;
	once = true;
	onceCheckpoint = true;

	physics.axisX = true;
	physics.axisY = true;
	physics.positiveSpeedY = true;
	physics.verlet = true;

	// Fx
	destroyedFx = app->entities->deadFx;
	jumpFx = app->entities->jumpFx;
	doubleJumpFx = app->entities->doubleJumpFx;
	fruitFx = app->entities->fruitFx;
	hitFx = app->entities->hitFx;
	slashFx = app->entities->slashFx;
	checkpointFx = app->entities->checkpointFx;

	// Textures
	playerHeart = app->entities->playerHeart;
	entityTex = app->entities->playerTex;

	// Animations
	for (int i = 0; i != 9; ++i)
	{
		idle.PushBack({ 10 + (playerSize * i),1329,56,73 });
	}
	idle.speed = 15.0f;
	idle.loop = true;

	for (int i = 0; i != 8; ++i)
	{
		run.PushBack({ 10 + (playerSize * i),1202,62,82 });
	}
	run.speed = 15.0f;
	run.loop = true;

	for (int i = 0; i != 2; ++i)
	{
		jumpPrep.PushBack({ 10 + (playerSize * i), 812, 60, 90 });
	}
	jumpPrep.speed = 15.0f;
	jumpPrep.loop = false;

	for (int i = 0; i != 4; ++i)
	{
		jumpMid.PushBack({ 10 + (2 * playerSize) + (playerSize * i),812,60,80 });
	}
	jumpMid.speed = 15.0f;
	jumpMid.loop = true;

	jumpLand.PushBack({ 10 + (playerSize * 6), 818, 60, 72 });
	jumpLand.PushBack({ 10 + (playerSize * 6), 818, 60, 72 });
	jumpLand.speed = 40.0f;
	jumpLand.loop = false;

	for (int i = 0; i != 7; ++i)
	{
		attack.PushBack({ (playerSize * i), 434, 104, 72 });
	}
	attack.speed = 15.0f;
	attack.loop = false;

	for (int i = 0; i != 3; ++i)
	{
		hit.PushBack({ 10 + (playerSize * i),52,56,73 });
	}
	hit.speed = 10.0f;
	hit.loop = false;

	for (int i = 0; i != 5; ++i)
	{
		death.PushBack({ 10 + (playerSize * i),192,88,66 });
	}
	death.speed = 15.0f;
	death.loop = false;

	wallJump.PushBack({ 630,170,73,79 });
	wallJump.speed = 0.0f;
	wallJump.loop = false;

	idle.Reset();
	run.Reset();
	jumpPrep.Reset();
	jumpMid.Reset();
	jumpLand.Reset();
	hit.Reset();
	attack.Reset();
	death.Reset();
	wallJump.Reset();

	currentAnim = &idle;
}

bool Player::Update(float dt)
{
	currentAnim->Update(dt);
	keyPressed = false;
	nextPos.x = entityRect.x;
	nextPos.y = entityRect.y;

	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN && currentAnim == &idle)
	{
		app->SaveRequest();
	}
	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN && currentAnim == &idle)
	{
		app->LoadRequest();
	}
	if (app->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
	{
		isDead = true;
	}
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		godMode = !godMode;
		physics.axisX = !physics.axisX;
		physics.axisY = !physics.axisY;
	}

	physics.CheckDirection();

	if (isDead == false)
	{
		if (godMode)
		{
			if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			{
				nextPos.y -= floor(250.0f * dt);
				physics.positiveSpeedY = false;
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			{
				nextPos.y += floor(250.0f * dt);
				physics.positiveSpeedY = true;
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT)
			{
				nextPos.x -= floor(250.0f * dt);
				currentAnim = &run;
				if (invert == false)
				{
					invert = true;
				}
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT)
			{
				nextPos.x += floor(250.0f * dt);
				currentAnim = &run;
				if (invert == true)
				{
					invert = false;
				}
				keyPressed = true;
			}
		}
		else if (hitCD == 0)
		{
			if (jumpCounter > 0)
			{
				if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
				{
					currentAnim = &jumpPrep;
					isJumping = true;
					if (jumpCounter == 2)
					{
						app->audio->PlayFx(jumpFx);
					}
					if (jumpCounter == 1)
					{
						app->audio->PlayFx(doubleJumpFx);
					}
					--jumpCounter;
					physics.speed.y = -400.0f;
				}
			}
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT)
			{
				if (currentAnim != &wallJump)
				{
					physics.speed.x = -250.0f;
				}
				if (!isJumping && !isAttacking)
				{
					currentAnim = &run;
					isAttacking = false;
				}
				if (invert == false && currentAnim != &wallJump)
				{
					invert = true;
				}
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT)
			{
				if (currentAnim != &wallJump)
				{
					physics.speed.x = 250.0f;
				}
				if (!isJumping && !isAttacking)
				{
					currentAnim = &run;
					isAttacking = false;
				}
				if (invert == true && currentAnim != &wallJump)
				{
					invert = false;
				}
				keyPressed = true;
			}
		}

		if (app->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN)
		{
			if (currentAnim != &attack)
			{
				app->audio->PlayFx(slashFx);
			}
			currentAnim = &attack;
			hurtBox = app->collisions->AddCollider(currentAnim->GetCurrentFrame(), Collider::Type::ATTACK, (Module*)app->entities);
			isAttacking = true;
		}

		if (hurtBox != nullptr)
		{
			if (invert && currentAnim == &attack)
			{
				hurtBox->SetPos(entityRect.x - 40, entityRect.y, currentAnim->GetCurrentFrame().w, currentAnim->GetCurrentFrame().h);
			}
			else
			{
				hurtBox->SetPos(entityRect.x, entityRect.y, currentAnim->GetCurrentFrame().w, currentAnim->GetCurrentFrame().h);
			}
		}

		//Animation reset to idle
		if (keyPressed == false)
		{
			physics.speed.x = 0.0f;
			if (isHit)
			{
				if (currentAnim->HasFinished())
				{
					isHit = false;
					hit.Reset();
					if (isJumping)
					{
						currentAnim = &jumpMid;
					}
				}
			}
			else if (isJumping == false && isAttacking == false)
			{
				run.Reset();
				currentAnim = &idle;
			}
		}

		// Attack reset to animation
		if (isAttacking)
		{
			if (currentAnim->HasFinished())
			{
				isAttacking = false;
				hurtBox->pendingToDelete = true;
				attack.Reset();
				if (isJumping)
				{
					currentAnim = &jumpMid;
				}
			}
		}

		// Jumping animation changes
		if (isJumping == true)
		{
			if (currentAnim->HasFinished())
			{
				if (currentAnim == &jumpPrep)
				{
					currentAnim = &jumpMid;
					jumpPrep.Reset();
				}
				if (currentAnim == &jumpLand)
				{
					isJumping = false;
					jumpCounter = 2;
					jumpLand.Reset();
				}
			}
		}

		physics.UpdatePhysics(nextPos, dt);
		physics.ResolveCollisions(entityRect, nextPos, invert);

		iPoint currentFrameTile = { entityRect.x / app->generalTileSize, entityRect.y / app->generalTileSize };

		// Animation correction
		if (app->map->GetTileProperty(currentFrameTile.x + 1, currentFrameTile.y, "CollisionId") == Collider::Type::SOLID
			&& app->map->GetTileProperty(currentFrameTile.x, currentFrameTile.y + 1, "CollisionId") != Collider::Type::SOLID
			&& !invert && currentAnim != &wallJump)
		{
			currentAnim = &wallJump;
			jumpCounter = 1;
		}
		else if (app->map->GetTileProperty((entityRect.x - 1) / app->generalTileSize, currentFrameTile.y, "CollisionId") == Collider::Type::SOLID
			&& app->map->GetTileProperty(currentFrameTile.x, currentFrameTile.y + 1, "CollisionId") != Collider::Type::SOLID
			&& invert && currentAnim != &wallJump)
		{
			currentAnim = &wallJump;
			jumpCounter = 1;
		}
		else if (app->map->GetTileProperty(currentFrameTile.x, currentFrameTile.y + 1, "CollisionId") == Collider::Type::SOLID && isJumping)
		{
			currentAnim = &jumpLand;
		}

		// Attack
		if (!isAttacking && currentAnim != &attack)
		{
			collider->SetPos(entityRect.x, entityRect.y, currentAnim->GetCurrentFrame().w, currentAnim->GetCurrentFrame().h);
		}

		// Checkpoint
		if (app->map->GetTileProperty(currentFrameTile.x, currentFrameTile.y, "CollisionId", true, true) == Collider::Type::CHECKPOINT)
		{
			if (onceCheckpoint)
			{
				spawnPos.x = entityRect.x;
				spawnPos.y = entityRect.y;
				app->audio->PlayFx(checkpointFx);
				onceCheckpoint = false;
			}
		}
		else
		{
			onceCheckpoint = true;
		}

		// Fruit collection
		if (app->map->GetTileProperty(currentFrameTile.x, currentFrameTile.y, "CollisionId", true, true) == Collider::Type::FRUIT)
		{
			if (app->map->GetTileProperty(currentFrameTile.x, currentFrameTile.y, "NoDraw", true, true) == 0)
			{
				lives++;
				app->map->SetTileProperty(entityRect.x / app->generalTileSize, entityRect.y / app->generalTileSize, "NoDraw", 1, true, true);
				app->audio->PlayFx(fruitFx);
				app->scene->scoreValue += 50;
			}
		}

		// Win condition
		if (app->map->GetTileProperty(currentFrameTile.x, currentFrameTile.y, "CollisionId", true, true) == Collider::Type::GOAL)
		{
			if (once)
			{
				app->audio->PlayMusic("Assets/Audio/Music/victory.ogg", 0.0f);
				spawnPos.x = 0;
				spawnPos.y = 0;
				once = false;
			}
			app->transition->FadeEffect((Module*)app->scene, (Module*)app->titleScene, false, floor(10000.0f * dt));
		}

		// Dead
		if (app->map->GetTileProperty(currentFrameTile.x, currentFrameTile.y + 1, "CollisionId") == Collider::Type::SPIKE && !godMode)
		{
			lives--;
			if (lives == 0)
			{
				isDead = true;
			}
			else
			{
				entityRect.x = spawnPos.x;
				entityRect.y = spawnPos.y;

				app->render->camera.x = -(spawnPos.x - app->render->camera.w / 2);
				app->render->camera.y = -(spawnPos.y - app->render->camera.h / 2 - app->generalTileSize);

				physics.speed.x = 0.0f;
				physics.speed.y = 0.0f;
				app->audio->PlayFx(hitFx);
				currentAnim = &hit;
				isHit = true;
			}
		}

		// Hit cooldown
		if (hitCD != 0)
		{
			hitCD--;
			if (invert)
			{
				entityRect.x += floor(250.0f * dt);
			}
			else
			{
				entityRect.x -= floor(250.0f * dt);
			}
		}
	}

	// Game over
	if (isDead)
	{
		currentAnim = &death;
		if (once)
		{
			once = false;
			app->audio->PlayFx(destroyedFx);
		}
		if (currentAnim->HasFinished())
		{
			app->transition->FadeEffect((Module*)app->scene, (Module*)app->deathScene, false, floor(10000.0f * dt));
		}
	}

	// Map borders
	if (entityRect.x <= 0)
	{
		entityRect.x = 0;
	}
	if ((entityRect.x + entityRect.w) > (app->map->data.width * app->map->data.tileWidth))
	{
		entityRect.x -= floor(250.0f * dt);
	}
	return true;
}

bool Player::Draw()
{
	if (invert)
	{
		if (isAttacking)
		{
			app->render->DrawTexture(entityTex, entityRect.x - 40, entityRect.y, false, &currentAnim->GetCurrentFrame(), invert);
		}
		else
		{
			app->render->DrawTexture(entityTex, entityRect.x + 8, entityRect.y, false, &currentAnim->GetCurrentFrame(), invert);
		}
	}
	else
	{
		app->render->DrawTexture(entityTex, entityRect.x, entityRect.y, false, &currentAnim->GetCurrentFrame(), invert);
	}

	if (app->render->drawAll)
	{
		app->render->DrawRectangle({ entityRect.x, entityRect.y, app->generalTileSize, app->generalTileSize }, 0, 255, 0, 100);
		if (hurtBox != nullptr && isAttacking)
		{
			app->render->DrawRectangle(hurtBox->rect, 255, 0, 255, 100);
		}
	}

	iPoint tmp(-app->render->camera.x, -app->render->camera.y);
	for (int i = lives; i > 0; i--)
	{
		app->render->DrawTexture(playerHeart, tmp.x + (i * 68) - app->generalTileSize, tmp.y);
	}

	return true;
}

/*
bool Player::Load(pugi::xml_node& save)
{
	LOG("Loading player coordinates");
	bool ret = true;

	entityRect.x = save.child("coordinates").attribute("x").as_int();
	entityRect.y = save.child("coordinates").attribute("y").as_int();
	score = save.child("score").attribute("value").as_int(0);
	lives = save.child("life").attribute("value").as_int(3);

	return ret;
}

bool Player::Save(pugi::xml_node& save)
{
	LOG("Saving player coordinates");
	bool ret = true;

	pugi::xml_node player = save.append_child("coordinates");
	player.append_attribute("x").set_value(entityRect.x);
	player.append_attribute("y").set_value(entityRect.y);
	save.append_child("score").append_attribute("value").set_value(score);
	save.append_child("life").append_attribute("value").set_value(lives);

	return ret;
}
*/

iPoint Player::GetSpawnPoint()
{
	iPoint ret = { 0,0 };

	// MapLayer
	ListItem<MapLayer*>* mapLayer = app->map->data.mapLayer.start;
	SString layerName = "Objects";
	while (mapLayer != NULL)
	{
		if (mapLayer->data->name == layerName)
		{
			break;
		}
		mapLayer = mapLayer->next;
	}

	// TileSet
	ListItem<TileSet*>* tileSet = app->map->data.tileSets.start;
	SString tileSetName = "level_1_tileset";
	while (tileSet != NULL)
	{
		if (tileSet->data->name == tileSetName)
		{
			break;
		}
		tileSet = tileSet->next;
	}

	// Gets coordinates
	int id = 0;
	for (int x = 0; x < app->map->data.width; ++x)
	{
		for (int y = 0; y < app->map->data.height; ++y)
		{
			id = (int)(mapLayer->data->Get(x, y) - tileSet->data->firstgId);
			if (id == 28)
			{
				ret.x = x * app->generalTileSize;
				ret.y = y * app->generalTileSize;
				return ret;
			}
		}
	}

	return ret;
}

void Player::OnCollision(Collider* c1, Collider* c2)
{
	if (c2->type == Collider::Type::ENEMY && hitCD == 0 && !isAttacking)
	{
		LOG("Enemy collision!\n");
		hitCD = 15;
		lives--;
		if (lives == 0)
		{
			isDead = true;
		}
		else
		{
			app->audio->PlayFx(hitFx);
			currentAnim = &hit;
			isHit = true;
		}
	}
}