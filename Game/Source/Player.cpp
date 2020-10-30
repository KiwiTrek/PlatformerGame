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

Player::Player() {}

Player::~Player() {}

void Player::Init()
{
	active = false;
}

bool Player::Start()
{
	spawnPoint = GetSpawnPoint();
	playerRect = { spawnPoint.x,spawnPoint.y,44,72 };
	jumpCounter = 2;

	godMode = false;
	keyPressed = false;
	isJumping = false;
	isDead = false;
	invert = false;
	debugDraw = false;
	once = true;

	playerPhysics.axisX = true;
	playerPhysics.axisY = true;

	playerTex = app->tex->Load("Assets/textures/characterSpritesheet.png");
	idle.Reset();
	run.Reset();
	jumpPrep.Reset();
	jumpMid.Reset();
	jumpLand.Reset();
	death.Reset();
	wallJump.Reset();

	deadFx = app->audio->LoadFx("Assets/audio/fx/lose.wav");
	jumpFx = app->audio->LoadFx("Assets/audio/fx/jump.wav");
	doubleJumpFx = app->audio->LoadFx("Assets/audio/fx/doubleJump.wav");
	fruitFx = app->audio->LoadFx("Assets/audio/fx/fruit.wav");
	app->audio->SetFxVolume(deadFx);
	app->audio->SetFxVolume(jumpFx);
	app->audio->SetFxVolume(doubleJumpFx);
	app->audio->SetFxVolume(fruitFx);
    return true;
}

bool Player::Awake(pugi::xml_node&)
{
	for (int i = 0; i != 9; ++i)
	{
		idle.PushBack({ 10 + (playerSize * i),1329,56,73 });
	}
	idle.speed = 0.1f;
	idle.loop = true;

	for (int i = 0; i != 8; ++i)
	{
		run.PushBack({ 10 + (playerSize * i),1202,62,82 });
	}
	run.speed = 0.5f;
	run.loop = true;

	for (int i = 0; i != 2; ++i)
	{
		jumpPrep.PushBack({ 10 + (playerSize * i), 812, 60, 90 });
	}
	jumpPrep.speed = 0.5f;
	jumpPrep.loop = false;

	for (int i = 0; i != 4; ++i)
	{
		jumpMid.PushBack({ 10 + (2 * playerSize) + (playerSize * i),812,60,80 });
	}
	jumpMid.speed = 0.1f;
	jumpMid.loop = true;

	jumpLand.PushBack({ 10 + (playerSize * 6), 818, 60, 72 });
	jumpLand.PushBack({ 10 + (playerSize * 6), 818, 60, 72 });
	jumpLand.speed = 3.0f;
	jumpLand.loop = false;

	for (int i = 0; i != 5; ++i)
	{
		death.PushBack({ 10 + (playerSize * i),192,88,66 });
	}
	death.speed = 0.3f;
	death.loop = false;

	wallJump.PushBack({ 648,170,55,79 });
	wallJump.speed = 0.0f;
	wallJump.loop = false;

	return true;
}

bool Player::PreUpdate()
{
	return true;
}

bool Player::Update(float dt)
{
	currentAnimation->Update();
	keyPressed = false;

	if (app->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
	{
		isDead = true;
	}
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		godMode = !godMode;
		playerPhysics.axisY = !playerPhysics.axisY;
	}

	if (speed.y >= 0) {
		positiveSpeedY = true;
	}
	else if (speed.y < 0) {
		positiveSpeedY = false;
	}
	if (speed.x >= 0) {
		positiveSpeedX = true;
	}
	else if (speed.x < 0) {
		positiveSpeedX = false;
	}

	if (isDead == false)
	{
		if (godMode)		//4 directional movement
		{
			if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			{
				playerRect.y -= 5;
				positiveSpeedY = false;
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			{
				playerRect.y += 5;
				positiveSpeedY = true;
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT)
			{
				playerRect.x -= 5;
				positiveSpeedX = false;
				currentAnimation = &run;
				if (invert == false)
				{
					invert = true;
				}
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT)
			{
				playerRect.x += 5;
				positiveSpeedX = true;
				currentAnimation = &run;
				if (invert == true)
				{
					invert = false;
				}
				keyPressed = true;
			}
		}
		else				//2 directional with jumping
		{
			if (jumpCounter > 0)
			{
				if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
				{
					if(currentAnimation == &wallJump){}
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
					speed.y = -500.0f;
				}
			}
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT)
			{
				playerRect.x -= 5;
				positiveSpeedX = false;
				if (!isJumping)
				{
					currentAnimation = &run;
				}
				if (invert == false)
				{
					invert = true;
				}
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT)
			{
				playerRect.x += 5;
				positiveSpeedX = true;
				if (!isJumping)
				{
					currentAnimation = &run;
				}
				if (invert == true)
				{
					invert = false;
				}
				keyPressed = true;
			}
		}

		if (keyPressed == false && isJumping == false)
		{
			run.Reset();
			currentAnimation = &idle;
		}

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

		//Physics
		playerPhysics.UpdatePhysics(playerRect.x, playerRect.y, speed.x, speed.y);

		//Collisions
		int x = playerRect.x / 64;
		int y = playerRect.y / 64;
		if (x < 0)
		{
			x = 0;
		}
		if (y < 0)
		{
			y = 0;
		}
		LOG("%d,%d", x, y);

		if (positiveSpeedX && positiveSpeedY)	//bottom right corner
		{
			CollisionType collisionType = GetCollisionType(GetTileProperty(x, y + 1, "CollisionId"), GetTileProperty(x + 1, y, "CollisionId"));
			if (collisionType == CollisionType::DOUBLE_SOLID)
			{
				playerRect.y = y * 2 * 64 - playerRect.y;
				speed.y = 0;
				if (isJumping)
				{
					currentAnimation = &jumpLand;
				}
				playerRect.x = 2 * 64 * (x + 1) - 64 * 2 - playerRect.x;
				speed.x = 0;
				LOG("BottomRight - DOUBLE_SOLID");
			}
			else if (collisionType == CollisionType::AIR_SOLID)
			{
				speed.y -= 25.0f;
				if (speed.y <= 0) {
					speed.y = 0;
				}
				jumpCounter = 1;
				currentAnimation = &wallJump;
				playerRect.x = 2 * 64 * (x + 1) - 64 * 2 - playerRect.x;
				LOG("BottomRight - AIR_SOLID");
			}
			else if (collisionType == CollisionType::SOLID_AIR)
			{
				playerRect.y = y * 2 * 64 - playerRect.y;
				speed.y = 0;
				speed.x = 0;
				if (isJumping)
				{
					currentAnimation = &jumpLand;
				}
				LOG("BottomRight - SOLID_AIR");
			}
		}
		else if (positiveSpeedX && !positiveSpeedY)	//top right corner
		{
			CollisionType collisionType = GetCollisionType(GetTileProperty(x, y, "CollisionId"), GetTileProperty(x + 1, y + 1, "CollisionId"));
			if (collisionType == CollisionType::DOUBLE_SOLID)
			{
				playerRect.y = (y + 1) * 2 * 64 - playerRect.y;
				speed.y = 0;
				playerRect.x = 2 * 64 * (x)-playerRect.x;
				LOG("TopRight - DOUBLE_SOLID");
			}
			else if (collisionType == CollisionType::AIR_SOLID)
			{
				speed.y -= 25.0f;
				if (speed.y <= 0) {
					speed.y = 0;
				}
				jumpCounter = 1;
				currentAnimation = &wallJump;
				playerRect.x = 2 * 64 * (x)-playerRect.x;
				LOG("TopRight - AIR_SOLID");
			}
			else if (collisionType == CollisionType::SOLID_AIR)
			{
				playerRect.y = (y + 1) * 2 * 64 - playerRect.y;
				speed.y = 0;
				LOG("TopRight - SOLID_AIR");
			}
		}
		else if (!positiveSpeedX && !positiveSpeedY)	//left top corner
		{
			CollisionType collisionType = GetCollisionType(GetTileProperty(x, y + 1, "CollisionId"), GetTileProperty(x + 1, y, "CollisionId"));
			if (collisionType == CollisionType::DOUBLE_SOLID)
			{
				playerRect.y = (y + 1) * 2 * 64 - playerRect.y;
				speed.y = 0;
				playerRect.x = 2 * 64 * (x + 1) - playerRect.x;
				LOG("TopLeft - DOUBLE_SOLID");
			}
			else if (collisionType == CollisionType::AIR_SOLID)
			{
				playerRect.y = (y + 1) * 2 * 64 - playerRect.y;
				speed.y = 0;
				LOG("TopLeft - AIR_SOLID");
			}
			else if (collisionType == CollisionType::SOLID_AIR)
			{
				speed.y -= 25.0f;
				if (speed.y <= 0) {
					speed.y = 0;
				}
				jumpCounter = 1;
				currentAnimation = &wallJump;
				playerRect.x = 2 * 64 * (x + 1) - playerRect.x;
				LOG("TopLeft - SOLID_AIR");
			}
		}
		else if (!positiveSpeedX && positiveSpeedY)	//left bottom corner
		{
			CollisionType collisionType = GetCollisionType(GetTileProperty(x, y, "CollisionId"), GetTileProperty(x + 1, y + 1, "CollisionId"));
			if (collisionType == CollisionType::DOUBLE_SOLID)
			{
				playerRect.y = y * 2 * 64 - playerRect.y;
				speed.y = 0;
				if (isJumping)
				{
					currentAnimation = &jumpLand;
				}
				playerRect.x = 2 * 64 * (x + 1) - playerRect.x;
				speed.x = 0;
				LOG("BottomLeft - DOUBLE_SOLID");
			}
			else if (collisionType == CollisionType::AIR_SOLID)
			{
				playerRect.y = y * 2 * 64 - playerRect.y;
				speed.y = 0;
				speed.x = 0;
				if (isJumping)
				{
					currentAnimation = &jumpLand;
				}
				LOG("BottomLeft - AIR_SOLID");
			}
			else if (collisionType == CollisionType::SOLID_AIR)
			{
				speed.y -= 25.0f;
				if (speed.y <= 0) {
					speed.y = 0;
				}
				jumpCounter = 1;
				currentAnimation = &wallJump;
				playerRect.x = 2 * 64 * (x + 1) - playerRect.x;
				LOG("BottomLeft - SOLID_AIR");
			}
		}

		// Fruit collection
		if (GetTileProperty(x, y, "CollisionId", true, true) == Collider::Type::FRUIT)
		{
			if(GetTileProperty(x, y, "NoDraw", true, true) == 0)
			{
				app->map->SetTileProperty(x, y, "NoDraw", 1, true, true);
				app->audio->PlayFx(fruitFx);
			}
		}

		// Win condition
		if (GetTileProperty(x, y, "CollisionId", true, true) == Collider::Type::GOAL)
		{
			if (once)
			{
				app->audio->PlayMusic("Assets/audio/music/Victory.ogg", 0.0f);
				once = false;
			}
			app->transition->FadeEffect((Module*)app->scene, (Module*)app->titleScene, false, 180.0f);
		}

		// Dead
		if (GetTileProperty(x, y + 1, "CollisionId") == Collider::Type::SPIKE)
		{
			isDead = true;
		}
	}

	
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
			app->transition->FadeEffect((Module*)app->scene, (Module*)app->deathScene, false, 60.0f);
		}
	}

    return true;
}

bool Player::PostUpdate()
{
	// Map borders
	if (playerRect.x <= 0)
	{
		playerRect.x = 0;
	}
	if ((playerRect.x + playerRect.w) > (app->map->data.w * app->map->data.tileW))
	{
		--playerRect.x;
	}

	app->render->DrawTexture(playerTex, playerRect.x, playerRect.y, false, &currentAnimation->GetCurrentFrame(), invert);
	if (app->render->drawAll)
	{
		app->render->DrawRectangle({ playerRect.x, playerRect.y, 64, 64 }, 0, 255, 0, 100);		// Player "hitbox"
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

	return true;
}

bool Player::Load(pugi::xml_node&)
{
    return true;
}

bool Player::Save(pugi::xml_node&)
{
    return true;
}

iPoint Player::GetSpawnPoint()
{
	iPoint ret = { 0,0 };

	// MapLayer
	ListItem <MapLayer*>* ML = app->map->data.mapLayer.start;
	SString layerName = "Objects";
	while (ML != NULL)
	{
		if (ML->data->name == layerName)
		{
			break;
		}
		ML = ML->next;
	}

	// TileSet
	ListItem <TileSet*>* T = app->map->data.tileSets.start;
	SString tileSetName = "Level1Tileset(64x64)";
	while (T != NULL)
	{
		if (T->data->name == tileSetName)
		{
			break;
		}
		T = T->next;
	}

	// Gets coordinates
	int id = 0;
	for (int x = 0; x < app->map->data.w; ++x)
	{
		for (int y = 0; y < app->map->data.h; ++y)
		{
			id = (int)(ML->data->Get(x, y) - T->data->firstgId);
			if (id == 28)
			{
				ret.x = x * 64;
				ret.y = y * 64;
				return ret;
			}
		}
	}

	return ret;
}

int Player::GetTileProperty(int x, int y, const char* property, bool notMovCollision, bool isObject) const
{
	int ret;
	// MapLayer
	ListItem <MapLayer*>* ML = app->map->data.mapLayer.start;
	SString layerName;
	if (isObject)
	{
		layerName = "Objects";
	}
	else
	{
		layerName = "Collisions";
	}
	while (ML != NULL)
	{
		if (ML->data->name == layerName)
		{
			break;
		}
		ML = ML->next;
	}

	// TileSet
	ListItem <TileSet*>* T = app->map->data.tileSets.start;
	SString tileSetName;
	if (notMovCollision)
	{
		tileSetName = "Level1Tileset(64x64)";
	}
	else
	{
		tileSetName = "MetaData";
	}
	while (T != NULL)
	{
		if (T->data->name == tileSetName)
		{
			break;
		}
		T = T->next;
	}

	// Gets CollisionId
	int id = (int)(ML->data->Get(x, y) - T->data->firstgId);
	if (id < 0)
	{
		ret = 0;
		return ret;
	}
	Tile* currentTile = T->data->GetPropList(id);
	ret = currentTile->properties.GetProperty(property,0);
	return ret;
}

Player::CollisionType Player::GetCollisionType(int A, int B) const
{
	if (A == Collider::Type::SOLID && B == Collider::Type::SOLID)
	{
		return CollisionType::DOUBLE_SOLID;
	}
	else if (A == Collider::Type::SOLID && B == Collider::Type::AIR)
	{
		return CollisionType::SOLID_AIR;
	}
	else if (A == Collider::Type::AIR && B == Collider::Type::AIR)
	{
		return CollisionType::DOUBLE_AIR;
	}
	else if (A == Collider::Type::AIR && B == Collider::Type::SOLID)
	{
		return CollisionType::AIR_SOLID;
	}
}