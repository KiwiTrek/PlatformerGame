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
	if (x == -1 && y == -1)
	{
		spawnPos = GetSpawnPoint();
		entityRect = { spawnPos.x, spawnPos.y, app->generalTileSize, app->generalTileSize };
	}
	else
	{
		if (firstCheckpoint)
		{
			spawnPos = { 77 * app->generalTileSize, 3 * app->generalTileSize };
		}
		entityRect = { x, y, app->generalTileSize, app->generalTileSize };
		updateCamera = true;
	}
	pendingToDelete = false;
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
	firstCheckpoint = true;
	heartLess = false;
	heartMore = false;

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

	for (int i = 0; i != 19; ++i)
	{
		heartDestroyed.PushBack({ (64 * i),0,64,64 });
	}
	heartDestroyed.speed = 35.0f;
	heartDestroyed.loop = false;

	for (int i = 0; i != 11; ++i)
	{
		heartRecovered.PushBack({ (64*10) - (64 * i),0,64,64 });
	}
	heartRecovered.speed = 35.0f;
	heartRecovered.loop = false;

	idle.Reset();
	run.Reset();
	jumpPrep.Reset();
	jumpMid.Reset();
	jumpLand.Reset();
	hit.Reset();
	attack.Reset();
	death.Reset();
	wallJump.Reset();
	heartRecovered.Reset();
	heartDestroyed.Reset();

	currentAnim = &idle;
	heartRecovered.currentFrame = heartRecovered.totalFrames - 1;
}

bool Player::Update(float dt)
{
	if (updateCamera == true)
	{
		updateCamera = false;
		app->render->camera.x = -(entityRect.x - app->render->camera.w / 2);
		app->render->camera.y = -(entityRect.y - app->render->camera.h / 2 - app->generalTileSize);
		if (app->render->camera.x >= 0)
		{
			app->render->camera.x = 0;
		}
		if (app->render->camera.y >= 0)
		{
			app->render->camera.y = 0;
		}

		while ((app->render->camera.w - app->render->camera.x) > (app->map->data.width * app->map->data.tileWidth))
		{
			app->render->camera.x += app->map->data.tileWidth;
		}
		while ((app->render->camera.h - app->render->camera.y) > (app->map->data.height * app->map->data.tileHeight))
		{
			app->render->camera.y += app->map->data.tileHeight;
		}
	}
	currentAnim->Update(dt);
	if (heartMore)
	{
		heartRecovered.Update(dt);
	}
	if (heartLess)
	{
		heartDestroyed.Update(dt);
	}
	keyPressed = false;
	nextPos.x = entityRect.x;
	nextPos.y = entityRect.y;

	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		isDead = true;
	}
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN && currentAnim == &idle)
	{
		app->SaveRequest();
	}
	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN && currentAnim == &idle)
	{
		app->LoadRequest();
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
		if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		{
			app->entities->pause = true;
		}

		if (godMode)
		{
			resultingMove = 250.0f;
			if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			{
				resultingMove *= 3;
			}
			if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			{
				nextPos.y -= floor(resultingMove * dt);
				physics.positiveSpeedY = false;
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			{
				nextPos.y += floor(resultingMove * dt);
				physics.positiveSpeedY = true;
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT)
			{
				nextPos.x -= floor(resultingMove * dt);
				currentAnim = &run;
				if (invert == false)
				{
					invert = true;
				}
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT)
			{
				nextPos.x += floor(resultingMove * dt);
				currentAnim = &run;
				if (invert == true)
				{
					invert = false;
				}
				keyPressed = true;
			}
			app->render->camera.x = -(entityRect.x - app->render->camera.w / 2 + entityRect.w);
			app->render->camera.y = -(entityRect.y - app->render->camera.h / 2 - app->generalTileSize + entityRect.h);
		}
		else if (!isHit)
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

		if (app->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN && currentAnim == &idle)
		{
			if (firstCheckpoint)
			{
				//2,11
				entityRect.x = 2 * app->generalTileSize;
				entityRect.y = 11 * app->generalTileSize;
				firstCheckpoint = false;
			}
			else
			{
				//77,3
				entityRect.x = 77 * app->generalTileSize;
				entityRect.y = 3 * app->generalTileSize;
				firstCheckpoint = true;
			}
			LOG("%d,%d", entityRect.x, entityRect.y);

			app->render->camera.x = -(entityRect.x - app->render->camera.w / 2);
			app->render->camera.y = -(entityRect.y - app->render->camera.h / 2 - app->generalTileSize);

			physics.speed.x = 0.0f;
			physics.speed.y = 0.0f;
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
				firstCheckpoint = true;
			}
		}
		else
		{
			onceCheckpoint = true;
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
			heartLess = true;
			if (lives == 0)
			{
				pendingToDelete = true;
				collider->pendingToDelete = true;
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

	LOG("%d,%d", entityRect.x, entityRect.y);

	return true;
}

bool Player::Draw()
{
	if (invert)
	{
		if (isAttacking)
		{
			app->render->DrawTexture(app->entities->playerTex, entityRect.x - 40, entityRect.y, false, &currentAnim->GetCurrentFrame(), invert);
		}
		else
		{
			app->render->DrawTexture(app->entities->playerTex, entityRect.x + 8, entityRect.y, false, &currentAnim->GetCurrentFrame(), invert);
		}
	}
	else
	{
		app->render->DrawTexture(app->entities->playerTex, entityRect.x, entityRect.y, false, &currentAnim->GetCurrentFrame(), invert);
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
	for (int i = lives - 1; i > 0; i--)
	{
		app->render->DrawTexture(app->entities->playerHeart, tmp.x + (i * 68) - app->generalTileSize, tmp.y, false, &heartRecovered.GetLastFrame());
	}
	if (heartMore && !heartLess)
	{
		app->render->DrawTexture(app->entities->playerHeart, tmp.x + (lives * 68) - app->generalTileSize, tmp.y, false, &heartRecovered.GetCurrentFrame());
		if (heartRecovered.HasFinished())
		{
			heartMore = false;
		}
	}
	else if (heartLess && !heartMore)
	{
		app->render->DrawTexture(app->entities->playerHeart, tmp.x + ((lives + 1) * 68) - app->generalTileSize, tmp.y, false, &heartDestroyed.GetCurrentFrame());
		app->render->DrawTexture(app->entities->playerHeart, tmp.x + (lives * 68) - app->generalTileSize, tmp.y, false, &heartRecovered.GetLastFrame());
		if (heartDestroyed.HasFinished())
		{
			heartLess = false;
		}
	}
	else
	{
		app->render->DrawTexture(app->entities->playerHeart, tmp.x + (lives * 68) - app->generalTileSize, tmp.y, false, &heartRecovered.GetLastFrame());
	}

	return true;
}

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
		hitCD = 50;
		heartLess = true;
		heartDestroyed.Reset();
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
	else if (c2->type == Collider::Type::FRUIT)
	{
		lives++;
		heartMore = true;
		heartRecovered.Reset();
	}
}