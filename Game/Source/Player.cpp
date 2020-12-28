#include "Player.h"

#include "Input.h"
#include "Render.h"
#include "Map.h"
#include "Audio.h"
#include "EntityManager.h"
#include "Collisions.h"

Player::Player(int x, int y, EntityManager* listener) : Entity(x, y, EntityType::PLAYER)
{
	this->listener = listener;
    spawnPos = GetSpawnPoint();
	collider = listener->collisions->AddCollider({ x, y, GENERAL_TILE_SIZE, GENERAL_TILE_SIZE }, Collider::Type::PLAYER, (Module*)listener);

	score = 0;
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


	//position = iPoint(x * 16, y * 16); //12,27
    //jumpSpeed = 200.0f;

    //width = 16;
    //height = 32;

    // Define Player animations
	for (int i = 0; i != 9; ++i)
	{
		idle.PushBack({ 10 + (GENERAL_TILE_SIZE * i),1329,56,73 });
	}
	idle.speed = 15.0f;
	idle.loop = true;

	for (int i = 0; i != 8; ++i)
	{
		run.PushBack({ 10 + (GENERAL_TILE_SIZE * i),1202,62,82 });
	}
	run.speed = 15.0f;
	run.loop = true;

	for (int i = 0; i != 2; ++i)
	{
		jumpPrep.PushBack({ 10 + (GENERAL_TILE_SIZE * i), 812, 60, 90 });
	}
	jumpPrep.speed = 15.0f;
	jumpPrep.loop = false;

	for (int i = 0; i != 4; ++i)
	{
		jumpMid.PushBack({ 10 + (2 * GENERAL_TILE_SIZE) + (GENERAL_TILE_SIZE * i),812,60,80 });
	}
	jumpMid.speed = 15.0f;
	jumpMid.loop = true;

	jumpLand.PushBack({ 10 + (GENERAL_TILE_SIZE * 6), 818, 60, 72 });
	jumpLand.PushBack({ 10 + (GENERAL_TILE_SIZE * 6), 818, 60, 72 });
	jumpLand.speed = 40.0f;
	jumpLand.loop = false;

	for (int i = 0; i != 7; ++i)
	{
		attack.PushBack({ (GENERAL_TILE_SIZE * i), 434, 104, 72 });
	}
	attack.speed = 15.0f;
	attack.loop = false;

	for (int i = 0; i != 3; ++i)
	{
		hit.PushBack({ 10 + (GENERAL_TILE_SIZE * i),52,56,73 });
	}
	hit.speed = 10.0f;
	hit.loop = false;

	for (int i = 0; i != 5; ++i)
	{
		death.PushBack({ 10 + (GENERAL_TILE_SIZE * i),192,88,66 });
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
}

bool Player::Update(Input* input, Map* map, Render* render, AudioManager* audio, float dt)
{
    //#define GRAVITY 400.0f
    //#define PLAYER_MOVE_SPEED 200.0f
    //#define PLAYER_JUMP_SPEED 350.0f

    //if (input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) position.y += (PLAYER_MOVE_SPEED * dt);
    //if (input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) position.y -= (PLAYER_MOVE_SPEED * dt);
    //if (input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) position.x -= (PLAYER_MOVE_SPEED * dt);
    //if (input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) position.x += (PLAYER_MOVE_SPEED * dt);

    //if (input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) position.y -= (PLAYER_JUMP_SPEED * dt);

    // Calculate gravity acceleration
    //jumpSpeed += GRAVITY * dt;
    //position.y += (jumpSpeed * dt);

	currentAnim->Update(dt);
	keyPressed = false;
	nextPos.x = collider->rect.x;
	nextPos.y = collider->rect.y;

	if (input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
	{
		isDead = true;
	}
	if (input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
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
			if (input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			{
				nextPos.y -= floor(250.0f * dt);
				physics.positiveSpeedY = false;
				keyPressed = true;
			}
			if (input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			{
				nextPos.y += floor(250.0f * dt);
				physics.positiveSpeedY = true;
				keyPressed = true;
			}
			if (input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT)
			{
				nextPos.x -= floor(250.0f * dt);
				currentAnim = &run;
				if (invert == false)
				{
					invert = true;
				}
				keyPressed = true;
			}
			if (input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT)
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
				if (input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
				{
					currentAnim = &jumpPrep;
					isJumping = true;
					if (jumpCounter == 2)
					{
						audio->PlayFx(listener->jumpFx);
					}
					if (jumpCounter == 1)
					{
						audio->PlayFx(listener->doubleJumpFx);
					}
					--jumpCounter;
					physics.speed.y = -400.0f;
				}
			}
			if (input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT)
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
			if (input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT)
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

		if (input->GetKey(SDL_SCANCODE_J) == KEY_DOWN)
		{
			if (currentAnim != &attack)
			{
				audio->PlayFx(listener->slashFx);
			}
			currentAnim = &attack;
			hurtBox = listener->collisions->AddCollider(currentAnim->GetCurrentFrame(), Collider::Type::ATTACK, (Module*)listener);
			isAttacking = true;
		}

		if (hurtBox != nullptr)
		{
			if (invert && currentAnim == &attack)
			{
				hurtBox->SetPos(collider->rect.x - 40, collider->rect.y, currentAnim->GetCurrentFrame().w, currentAnim->GetCurrentFrame().h);
			}
			else
			{
				hurtBox->SetPos(collider->rect.x, collider->rect.y, currentAnim->GetCurrentFrame().w, currentAnim->GetCurrentFrame().h);
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

		// Physics & Collisions
		//physics.UpdatePhysics(nexPos, dt);
		//physics.ResolveCollisions(map, nextFrame, invert);

		iPoint currentFrameTile = { collider->rect.x / GENERAL_TILE_SIZE, collider->rect.y / GENERAL_TILE_SIZE };

		// Animation correction
		if (map->GetTileProperty(currentFrameTile.x + 1, currentFrameTile.y, "CollisionId") == Collider::Type::SOLID
			&& map->GetTileProperty(currentFrameTile.x, currentFrameTile.y + 1, "CollisionId") != Collider::Type::SOLID
			&& !invert && currentAnim != &wallJump)
		{
			currentAnim = &wallJump;
			jumpCounter = 1;
		}
		else if (map->GetTileProperty((collider->rect.x - 1) / GENERAL_TILE_SIZE, currentFrameTile.y, "CollisionId") == Collider::Type::SOLID
			&& map->GetTileProperty(currentFrameTile.x, currentFrameTile.y + 1, "CollisionId") != Collider::Type::SOLID
			&& invert && currentAnim != &wallJump)
		{
			currentAnim = &wallJump;
			jumpCounter = 1;
		}
		else if (map->GetTileProperty(currentFrameTile.x, currentFrameTile.y + 1, "CollisionId") == Collider::Type::SOLID && isJumping)
		{
			currentAnim = &jumpLand;
		}

		// Attack
		if (!isAttacking && currentAnim != &attack)
		{
			collider->SetPos(collider->rect.x, collider->rect.y, currentAnim->GetCurrentFrame().w, currentAnim->GetCurrentFrame().h);
		}

		// Checkpoint
		if (map->GetTileProperty(currentFrameTile.x, currentFrameTile.y, "CollisionId", true, true) == Collider::Type::CHECKPOINT)
		{
			if (onceCheckpoint)
			{
				spawnPos.x = collider->rect.x;
				spawnPos.y = collider->rect.y;
				audio->PlayFx(listener->checkpointFx);
				onceCheckpoint = false;
			}
		}
		else
		{
			onceCheckpoint = true;
		}

		// Fruit collection
		if (map->GetTileProperty(currentFrameTile.x, currentFrameTile.y, "CollisionId", true, true) == Collider::Type::FRUIT)
		{
			if (map->GetTileProperty(currentFrameTile.x, currentFrameTile.y, "NoDraw", true, true) == 0)
			{
				lives++;
				map->SetTileProperty(collider->rect.x / GENERAL_TILE_SIZE, collider->rect.y / GENERAL_TILE_SIZE, "NoDraw", 1, true, true);
				audio->PlayFx(listener->fruitFx);
				score += 50;
			}
		}

		// Win condition
		if (map->GetTileProperty(currentFrameTile.x, currentFrameTile.y, "CollisionId", true, true) == Collider::Type::GOAL)
		{
			if (once)
			{
				audio->PlayMusic("Assets/Audio/Music/victory.ogg", 0.0f);
				spawnPos.x = 0;
				spawnPos.y = 0;
				once = false;
			}
			//transition->FadeEffect((Module*)app->scene, (Module*)app->titleScene, false, floor(10000.0f * dt));
		}

		// Dead
		if (map->GetTileProperty(currentFrameTile.x, currentFrameTile.y + 1, "CollisionId") == Collider::Type::SPIKE && !godMode)
		{
			lives--;
			if (lives == 0)
			{
				isDead = true;
			}
			else
			{
				collider->rect.x = spawnPos.x;
				collider->rect.y = spawnPos.y;

				render->camera.x = -(spawnPos.x - render->camera.w / 2);
				render->camera.y = -(spawnPos.y - render->camera.h / 2 - GENERAL_TILE_SIZE);

				physics.speed.x = 0.0f;
				physics.speed.y = 0.0f;
				audio->PlayFx(listener->hitFx);
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
				collider->rect.x += floor(250.0f * dt);
			}
			else
			{
				collider->rect.x -= floor(250.0f * dt);
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
			audio->PlayFx(listener->deadFx);
		}
		if (currentAnim->HasFinished())
		{
			//transition->FadeEffect((Module*)app->scene, (Module*)app->deathScene, false, floor(10000.0f * dt));
		}
	}

	// Map borders
	if (collider->rect.x <= 0)
	{
		collider->rect.x = 0;
	}
	if ((collider->rect.x + collider->rect.w) > (map->data.width * map->data.tileWidth))
	{
		collider->rect.x -= floor(250.0f * dt);
	}

    return true;
}

bool Player::Draw(Render* render)
{
    // TODO: Calculate the corresponding rectangle depending on the
    // animation state and animation frame
    //SDL_Rect rec = { 0 };
    //render->DrawTexture(texture, position.x, position.y, rec);

    render->DrawRectangle(GetBounds(), { 255, 0, 0, 255 });

    return false;
}

void Player::SetTexture(SDL_Texture *tex)
{
    texture = tex;
}

SDL_Rect Player::GetBounds()
{
    return { position.x, position.y, GENERAL_TILE_SIZE, GENERAL_TILE_SIZE };
}

bool Player::IsIdle()
{
	return(currentAnim == &idle);
}

iPoint Player::GetSpawnPoint()
{
	iPoint ret = { 0,0 };

	// MapLayer
	ListItem<MapLayer*>* mapLayer = listener->map->data.mapLayer.start;
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
	ListItem<TileSet*>* tileSet = listener->map->data.tileSets.start;
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
	for (int x = 0; x < listener->map->data.width; ++x)
	{
		for (int y = 0; y < listener->map->data.height; ++y)
		{
			id = (int)(mapLayer->data->Get(x, y) - tileSet->data->firstgid);
			if (id == 28)
			{
				ret.x = x * GENERAL_TILE_SIZE;
				ret.y = y * GENERAL_TILE_SIZE;
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
        hitCD = 15;
        lives--;
        if (lives == 0)
        {
            isDead = true;
        }
        else
        {
            currentAnim = &hit;
            isHit = true;
        }
    }
}