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

#include "Log.h"

Player::Player()
{
	name.Create("player");
}

Player::~Player()
{}

void Player::Init()
{
	active = false;
	spawnPoint = { 0,0 };
}

bool Player::Awake(pugi::xml_node& config)
{
	playerSize = config.child("playerSize").attribute("value").as_int(0);

	folderTexture.Create(config.child("folderTexture").child_value());
	folderAudioFx.Create(config.child("folderAudioFx").child_value());

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

	return true;
}

bool Player::Start()
{
	spawnPoint = GetSpawnPoint();
	playerRect = { spawnPoint.x, spawnPoint.y, app->generalTileSize, app->generalTileSize };
	playerCollider = app->collisions->AddCollider(playerRect, Collider::Type::PLAYER, this);

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

	playerPhysics.axisX = true;
	playerPhysics.axisY = true;
	playerPhysics.positiveSpeedY = true;
	playerPhysics.verlet = true;

	SString tmp("%s%s", folderTexture.GetString(), "character_spritesheet.png");
	playerTex = app->tex->Load(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "heart.png");
	playerHeart = app->tex->Load(tmp.GetString());

	idle.Reset();
	run.Reset();
	jumpPrep.Reset();
	jumpMid.Reset();
	jumpLand.Reset();
	hit.Reset();
	attack.Reset();
	death.Reset();
	wallJump.Reset();

	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "lose.wav");
	deadFx = app->audio->LoadFx(tmp.GetString());
	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "jump.wav");
	jumpFx = app->audio->LoadFx(tmp.GetString());
	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "double_jump.wav");
	doubleJumpFx = app->audio->LoadFx(tmp.GetString());
	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "fruit.wav");
	fruitFx = app->audio->LoadFx(tmp.GetString());
	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "hit.wav");
	hitFx = app->audio->LoadFx(tmp.GetString());
	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "slash.wav");
	slashFx = app->audio->LoadFx(tmp.GetString());
	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "checkpoint.wav");
	checkpointFx = app->audio->LoadFx(tmp.GetString());

	app->audio->SetFxVolume(deadFx);
	app->audio->SetFxVolume(jumpFx);
	app->audio->SetFxVolume(doubleJumpFx);
	app->audio->SetFxVolume(fruitFx);
	app->audio->SetFxVolume(hitFx);
	app->audio->SetFxVolume(slashFx);
	app->audio->SetFxVolume(checkpointFx);

	return true;
}

bool Player::Update(float dt)
{
	currentAnimation->Update(dt);
	keyPressed = false;
	nextFrame.x = playerRect.x;
	nextFrame.y = playerRect.y;

	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN && currentAnimation == &idle)
	{
		app->SaveRequest();
	}
	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN && currentAnimation == &idle)
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
		playerPhysics.axisX = !playerPhysics.axisX;
		playerPhysics.axisY = !playerPhysics.axisY;
	}
	if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
	{
		app->CapRequest();
	}

	playerPhysics.CheckDirection();

	if (isDead == false)
	{
		if (godMode)
		{
			if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			{
				nextFrame.y -= floor(250.0f * dt);
				playerPhysics.positiveSpeedY = false;
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			{
				nextFrame.y += floor(250.0f * dt);
				playerPhysics.positiveSpeedY = true;
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT)
			{
				nextFrame.x -= floor(250.0f * dt);
				currentAnimation = &run;
				if (invert == false)
				{
					invert = true;
				}
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT)
			{
				nextFrame.x += floor(250.0f * dt);
				currentAnimation = &run;
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
					currentAnimation = &jumpPrep;
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
					playerPhysics.speed.y = -400.0f;
				}
			}
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT)
			{
				if (currentAnimation != &wallJump)
				{
					playerPhysics.speed.x = -250.0f;
				}
				if (!isJumping && !isAttacking)
				{
					currentAnimation = &run;
					isAttacking = false;
				}
				if (invert == false && currentAnimation != &wallJump)
				{
					invert = true;
				}
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT)
			{
				if (currentAnimation != &wallJump)
				{
					playerPhysics.speed.x = 250.0f;
				}
				if (!isJumping && !isAttacking)
				{
					currentAnimation = &run;
					isAttacking = false;
				}
				if (invert == true && currentAnimation != &wallJump)
				{
					invert = false;
				}
				keyPressed = true;
			}
		}

		if (app->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN)
		{
			if (currentAnimation != &attack)
			{
				app->audio->PlayFx(slashFx);
			}
			currentAnimation = &attack;
			hurtBox = app->collisions->AddCollider(currentAnimation->GetCurrentFrame(), Collider::Type::ATTACK, this);
			isAttacking = true;
		}

		if (hurtBox != nullptr)
		{
			if (invert && currentAnimation == &attack)
			{
				hurtBox->SetPos(playerRect.x - 40, playerRect.y, currentAnimation->GetCurrentFrame().w, currentAnimation->GetCurrentFrame().h);
			}
			else
			{
				hurtBox->SetPos(playerRect.x, playerRect.y, currentAnimation->GetCurrentFrame().w, currentAnimation->GetCurrentFrame().h);
			}
		}

		//Animation reset to idle
		if (keyPressed == false)
		{
			playerPhysics.speed.x = 0.0f;
			if (isHit)
			{
				if (currentAnimation->HasFinished())
				{
					isHit = false;
					hit.Reset();
					if (isJumping)
					{
						currentAnimation = &jumpMid;
					}
				}
			}
			else if (isJumping == false && isAttacking == false)
			{
				run.Reset();
				currentAnimation = &idle;
			}
		}

		// Attack reset to animation
		if (isAttacking)
		{
			if (currentAnimation->HasFinished())
			{
				isAttacking = false;
				hurtBox->pendingToDelete = true;
				attack.Reset();
				if (isJumping)
				{
					currentAnimation = &jumpMid;
				}
			}
		}

		// Jumping animation changes
		if (isJumping == true)
		{
			if (currentAnimation->HasFinished())
			{
				if (currentAnimation == &jumpPrep)
				{
					currentAnimation = &jumpMid;
					jumpPrep.Reset();
				}
				if (currentAnimation == &jumpLand)
				{
					isJumping = false;
					jumpCounter = 2;
					jumpLand.Reset();
				}
			}
		}

		// Physics & Collisions
		playerPhysics.UpdatePhysics(nextFrame, dt);
		playerPhysics.ResolveCollisions(playerRect, nextFrame, invert);

		iPoint currentFrameTile = { playerRect.x / app->generalTileSize, playerRect.y / app->generalTileSize };

		// Animation correction
		if (app->map->GetTileProperty(currentFrameTile.x + 1, currentFrameTile.y, "CollisionId") == Collider::Type::SOLID
			&& app->map->GetTileProperty(currentFrameTile.x, currentFrameTile.y + 1, "CollisionId") != Collider::Type::SOLID
			&& !invert && currentAnimation != &wallJump)
		{
			currentAnimation = &wallJump;
			jumpCounter = 1;
		}
		else if (app->map->GetTileProperty((playerRect.x - 1) / app->generalTileSize, currentFrameTile.y, "CollisionId") == Collider::Type::SOLID
			&& app->map->GetTileProperty(currentFrameTile.x, currentFrameTile.y + 1, "CollisionId") != Collider::Type::SOLID
			&& invert && currentAnimation != &wallJump)
		{
			currentAnimation = &wallJump;
			jumpCounter = 1;
		}
		else if (app->map->GetTileProperty(currentFrameTile.x, currentFrameTile.y + 1, "CollisionId") == Collider::Type::SOLID && isJumping)
		{
			currentAnimation = &jumpLand;
		}

		// Attack
		if (!isAttacking && currentAnimation != &attack)
		{
			playerCollider->SetPos(playerRect.x, playerRect.y, currentAnimation->GetCurrentFrame().w, currentAnimation->GetCurrentFrame().h);
		}

		// Checkpoint
		if (app->map->GetTileProperty(currentFrameTile.x, currentFrameTile.y, "CollisionId", true, true) == Collider::Type::CHECKPOINT)
		{
			if (onceCheckpoint)
			{
				spawnPoint.x = playerRect.x;
				spawnPoint.y = playerRect.y;
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
				app->map->SetTileProperty(playerRect.x / app->generalTileSize, playerRect.y / app->generalTileSize, "NoDraw", 1, true, true);
				app->audio->PlayFx(fruitFx);
				score += 50;
			}
		}

		// Win condition
		if (app->map->GetTileProperty(currentFrameTile.x, currentFrameTile.y, "CollisionId", true, true) == Collider::Type::GOAL)
		{
			if (once)
			{
				app->audio->PlayMusic("Assets/Audio/Music/victory.ogg", 0.0f);
				spawnPoint.x = 0;
				spawnPoint.y = 0;
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
				playerRect.x = spawnPoint.x;
				playerRect.y = spawnPoint.y;

				app->render->camera.x = -(spawnPoint.x - app->render->camera.w / 2);
				app->render->camera.y = -(spawnPoint.y - app->render->camera.h / 2 - app->generalTileSize);

				playerPhysics.speed.x = 0.0f;
				playerPhysics.speed.y = 0.0f;
				app->audio->PlayFx(hitFx);
				currentAnimation = &hit;
				isHit = true;
			}
		}

		// Hit cooldown
		if (hitCD != 0)
		{
			hitCD--;
			if (invert)
			{
				playerRect.x += floor(250.0f * dt);
			}
			else
			{
				playerRect.x -= floor(250.0f * dt);
			}
		}
	}

	// Game over
	if (isDead)
	{
		currentAnimation = &death;
		if (once)
		{
			once = false;
			app->audio->PlayFx(deadFx);
		}
		if (currentAnimation->HasFinished())
		{
			app->transition->FadeEffect((Module*)app->scene, (Module*)app->deathScene, false, floor(10000.0f * dt));
		}
	}

	// Map borders
	if (playerRect.x <= 0)
	{
		playerRect.x = 0;
	}
	if ((playerRect.x + playerRect.w) > (app->map->data.width * app->map->data.tileWidth))
	{
		playerRect.x -= floor(250.0f * dt);
	}
	return true;
}

bool Player::PostUpdate()
{
	if (invert)
	{
		if (isAttacking)
		{
			app->render->DrawTexture(playerTex, playerRect.x - 40, playerRect.y, false, &currentAnimation->GetCurrentFrame(), invert);
		}
		else
		{
			app->render->DrawTexture(playerTex, playerRect.x + 8, playerRect.y, false, &currentAnimation->GetCurrentFrame(), invert);
		}
	}
	else
	{
		app->render->DrawTexture(playerTex, playerRect.x, playerRect.y, false, &currentAnimation->GetCurrentFrame(), invert);
	}

	if (app->render->drawAll)
	{
		app->render->DrawRectangle({ playerRect.x, playerRect.y, app->generalTileSize, app->generalTileSize }, 0, 255, 0, 100);
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

bool Player::CleanUp()
{
	app->tex->UnLoad(playerTex);

	app->audio->UnloadFx(deadFx);
	app->audio->UnloadFx(doubleJumpFx);
	app->audio->UnloadFx(fruitFx);
	app->audio->UnloadFx(jumpFx);
	app->audio->UnloadFx(hitFx);
	app->audio->UnloadFx(slashFx);
	app->audio->UnloadFx(checkpointFx);

	return true;
}

bool Player::Load(pugi::xml_node& save)
{
	LOG("Loading player coordinates");
	bool ret = true;

	playerRect.x = save.child("coordinates").attribute("x").as_int();
	playerRect.y = save.child("coordinates").attribute("y").as_int();
	score = save.child("score").attribute("value").as_int(0);
	lives = save.child("life").attribute("value").as_int(3);

	return ret;
}

bool Player::Save(pugi::xml_node& save)
{
	LOG("Saving player coordinates");
	bool ret = true;

	pugi::xml_node player = save.append_child("coordinates");
	player.append_attribute("x").set_value(playerRect.x);
	player.append_attribute("y").set_value(playerRect.y);
	save.append_child("score").append_attribute("value").set_value(score);
	save.append_child("life").append_attribute("value").set_value(lives);

	return ret;
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
		hitCD = 15;
		lives--;
		if (lives == 0)
		{
			isDead = true;
		}
		else
		{
			app->audio->PlayFx(hitFx);
			currentAnimation = &hit;
			isHit = true;
		}
	}
}