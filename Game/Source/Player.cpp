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
}

bool Player::Start()
{
	spawnPoint = GetSpawnPoint();
	playerRect = { spawnPoint.x, spawnPoint.y, idle.GetCurrentFrame().w, idle.GetCurrentFrame().h };
	jumpCounter = 2;

	godMode = false;
	isDead = false;
	keyPressed = false;
	isJumping = false;
	invert = false;
	debugDraw = false;
	once = true;
	onceAnim = true;
	playerPhysics.axisX = true;
	playerPhysics.axisY = true;
	positiveSpeedX = true;
	positiveSpeedY = true;

	SString tmp("%s%s", folderTexture.GetString(), "character_spritesheet.png");
	playerTex = app->tex->Load(tmp.GetString());

	idle.Reset();
	run.Reset();
	jumpPrep.Reset();
	jumpMid.Reset();
	jumpLand.Reset();
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

	app->audio->SetFxVolume(deadFx);
	app->audio->SetFxVolume(jumpFx);
	app->audio->SetFxVolume(doubleJumpFx);
	app->audio->SetFxVolume(fruitFx);

	return true;
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
	idle.loop = true;

	for (int i = 0; i != 8; ++i)
	{
		run.PushBack({ 10 + (playerSize * i),1202,62,82 });
	}
	run.loop = true;

	for (int i = 0; i != 2; ++i)
	{
		jumpPrep.PushBack({ 10 + (playerSize * i), 812, 60, 90 });
	}
	jumpPrep.loop = false;

	for (int i = 0; i != 4; ++i)
	{
		jumpMid.PushBack({ 10 + (2 * playerSize) + (playerSize * i),812,60,80 });
	}
	jumpMid.loop = true;

	jumpLand.PushBack({ 10 + (playerSize * 6), 818, 60, 72 });
	jumpLand.PushBack({ 10 + (playerSize * 6), 818, 60, 72 });
	jumpLand.loop = false;

	for (int i = 0; i != 5; ++i)
	{
		death.PushBack({ 10 + (playerSize * i),192,88,66 });
	}
	death.loop = false;

	wallJump.PushBack({ 630,170,73,79 });
	wallJump.loop = false;

	return true;
}

bool Player::PreUpdate()
{
	return true;
}

bool Player::Update(float dt)
{
	if(onceAnim)
	{
		// NEEDS FIXING (with framerate)
		onceAnim = false;
		idle.speed = 2.5f * dt;
		run.speed = 2.5f * dt;
		jumpPrep.speed = 2.5f * dt;
		jumpMid.speed = 2.5f * dt;
		jumpLand.speed = 7.5f * dt;
		death.speed = 2.5f * dt;
		wallJump.speed = 0.0f;
	}
	currentAnimation->Update();
	keyPressed = false;

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

	// To know what direction the velocity is going
	if (speed.y >= 0.0f)
	{
		positiveSpeedY = true;
	}
	else if (speed.y < 0.0f)
	{
		positiveSpeedY = false;
	}

	if (speed.x >= 0.0f)
	{
		positiveSpeedX = true;
	}
	else if (speed.x < 0.0f)
	{
		positiveSpeedX = false;
	}

	if (isDead == false)
	{
		if (godMode)
		{
			if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			{
				playerRect.y -= floor(250.0f * dt);
				positiveSpeedY = false;
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			{
				playerRect.y += floor(250.0f * dt);
				positiveSpeedY = true;
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT)
			{
				playerRect.x -= floor(250.0f * dt);
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
				playerRect.x += floor(250.0f * dt);
				positiveSpeedX = true;
				currentAnimation = &run;
				if (invert == true)
				{
					invert = false;
				}
				keyPressed = true;
			}
		}
		else
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
					speed.y = -450.0f;
				}
			}
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT)
			{
				if (currentAnimation != &wallJump)
				{
					// playerRect.x -= floor(250.0f * dt);
					speed.x = -250.0f;
				}
				positiveSpeedX = false;
				if (!isJumping)
				{
					currentAnimation = &run;
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
					// playerRect.x += floor(250.0f * dt);
					speed.x = 250.0f;
				}
				positiveSpeedX = true;
				if (!isJumping)
				{
					currentAnimation = &run;
				}
				if (invert == true && currentAnimation != &wallJump)
				{
					invert = false;
				}
				keyPressed = true;
			}
		}

		if (keyPressed == false)
		{
			speed.x = 0;
			if (isJumping == false)
			{
				run.Reset();
				currentAnimation = &idle;
			}
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

		// Physics
		playerPhysics.UpdatePhysics(playerRect.x, playerRect.y, speed.x, speed.y, dt);

		// Collisions
		int x = playerRect.x / 64;
		int y = playerRect.y / 64;

		// So it doesn't check for non-existant tiles
		if (x < 0)
		{
			x = 0;
		}
		if (y < 0)
		{
			y = 0;
		}
		//LOG("%d,%d", x, y);

		if (positiveSpeedX && positiveSpeedY)	// Bottom right corner
		{
			CollisionType collisionType = GetCollisionType(GetTileProperty(x, y + 1, "CollisionId"), GetTileProperty(x + 1, y, "CollisionId"));
			if (collisionType == CollisionType::DOUBLE_SOLID)
			{
				playerRect.y = y * 2 * 64 - playerRect.y;
				speed.y = 0.0f;
				if (isJumping)
				{
					currentAnimation = &jumpLand;
				}
				playerRect.x = 2 * 64 * (x + 1) - 64 * 2 - playerRect.x;
				speed.x = 0.0f;
				//LOG("BottomRight - DOUBLE_SOLID");
			}
			else if (collisionType == CollisionType::AIR_SOLID)
			{
				speed.y -= 450.0f;
				if (speed.y <= 0.0f)
				{
					speed.y = 0.0f;
				}
				jumpCounter = 1;
				currentAnimation = &wallJump;
				playerRect.x = 2 * 64 * (x + 1) - 64 * 2 - playerRect.x;
				speed.x = 0.0f;
				//LOG("BottomRight - AIR_SOLID");
			}
			else if (collisionType == CollisionType::SOLID_AIR)
			{
				playerRect.y = y * 2 * 64 - playerRect.y;
				speed.y = 0.0f;
				speed.x = 0.0f;
				if (isJumping)
				{
					currentAnimation = &jumpLand;
				}
				//LOG("BottomRight - SOLID_AIR");
			}
		}
		else if (positiveSpeedX && !positiveSpeedY)	// Top right corner
		{
			CollisionType collisionType = GetCollisionType(GetTileProperty(x, y, "CollisionId"), GetTileProperty(x + 1, y + 1, "CollisionId"));
			if (collisionType == CollisionType::DOUBLE_SOLID)
			{
				playerRect.y = (y + 1) * 2 * 64 - playerRect.y;
				speed.y = 0.0f;
				playerRect.x = 2 * 64 * (x)-playerRect.x;
				//LOG("TopRight - DOUBLE_SOLID");
			}
			else if (collisionType == CollisionType::AIR_SOLID)
			{
				speed.y -= 450.0f;
				if (speed.y <= 0.0f)
				{
					speed.y = 0.0f;
				}
				jumpCounter = 1;
				currentAnimation = &wallJump;
				playerRect.x = 2 * 64 * (x)-playerRect.x;
				speed.x = 0.0f;
				//LOG("TopRight - AIR_SOLID");
			}
			else if (collisionType == CollisionType::SOLID_AIR)
			{
				playerRect.y = (y + 1) * 2 * 64 - playerRect.y;
				speed.y = 0.0f;
				//LOG("TopRight - SOLID_AIR");
			}
		}
		else if (!positiveSpeedX && !positiveSpeedY)	// Left top corner
		{
			CollisionType collisionType = GetCollisionType(GetTileProperty(x, y + 1, "CollisionId"), GetTileProperty(x + 1, y, "CollisionId"));
			if (collisionType == CollisionType::DOUBLE_SOLID)
			{
				playerRect.y = (y + 1) * 2 * 64 - playerRect.y;
				speed.y = 0.0f;
				playerRect.x = 2 * 64 * (x + 1) - playerRect.x;
				//LOG("TopLeft - DOUBLE_SOLID");
			}
			else if (collisionType == CollisionType::AIR_SOLID)
			{
				playerRect.y = (y + 1) * 2 * 64 - playerRect.y;
				speed.y = 0.0f;
				//LOG("TopLeft - AIR_SOLID");
			}
			else if (collisionType == CollisionType::SOLID_AIR)
			{
				speed.y -= 450.0f;
				if (speed.y <= 0.0f)
				{
					speed.y = 0.0f;
				}
				jumpCounter = 1;
				currentAnimation = &wallJump;
				playerRect.x = 2 * 64 * (x + 1) - playerRect.x;
				speed.x = 0.0f;
				//LOG("TopLeft - SOLID_AIR");
			}
		}
		else if (!positiveSpeedX && positiveSpeedY)	// Left bottom corner
		{
			CollisionType collisionType = GetCollisionType(GetTileProperty(x, y, "CollisionId"), GetTileProperty(x + 1, y + 1, "CollisionId"));
			if (collisionType == CollisionType::DOUBLE_SOLID)
			{
				playerRect.y = y * 2 * 64 - playerRect.y;
				speed.y = 0.0f;
				if (isJumping)
				{
					currentAnimation = &jumpLand;
				}
				playerRect.x = 2 * 64 * (x + 1) - playerRect.x;
				speed.x = 0.0f;
				//LOG("BottomLeft - DOUBLE_SOLID");
			}
			else if (collisionType == CollisionType::AIR_SOLID)
			{
				playerRect.y = y * 2 * 64 - playerRect.y;
				speed.y = 0.0f;
				speed.x = 0.0f;
				if (isJumping)
				{
					currentAnimation = &jumpLand;
				}
				//LOG("BottomLeft - AIR_SOLID");
			}
			else if (collisionType == CollisionType::SOLID_AIR)
			{
				speed.y -= 450.0f;
				if (speed.y <= 0.0f)
				{
					speed.y = 0.0f;
				}
				jumpCounter = 1;
				currentAnimation = &wallJump;
				playerRect.x = 2 * 64 * (x + 1) - playerRect.x;
				speed.x = 0.0f;
				//LOG("BottomLeft - SOLID_AIR");
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
				app->audio->PlayMusic("Assets/Audio/Music/victory.ogg", 0.0f);
				once = false;
			}
			app->transition->FadeEffect((Module*)app->scene, (Module*)app->titleScene, false, floor(3000.0f * dt));
		}

		// Dead
		if (GetTileProperty(x, y + 1, "CollisionId") == Collider::Type::SPIKE && !godMode)
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
			app->transition->FadeEffect((Module*)app->scene, (Module*)app->deathScene, false, floor(1200.0f * dt));
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
	app->render->DrawTexture(playerTex, playerRect.x, playerRect.y, false, &currentAnimation->GetCurrentFrame(), invert);
	if (app->render->drawAll)
	{
		app->render->DrawRectangle({ playerRect.x, playerRect.y, 64, 64 }, 0, 255, 0, 100);
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

bool Player::Load(pugi::xml_node& save)
{
	LOG("Loading player coordinates");
	bool ret = true;

	playerRect.x = save.child("coordinates").attribute("x").as_int();
	playerRect.y = save.child("coordinates").attribute("y").as_int();

	return ret;
}

bool Player::Save(pugi::xml_node& save)
{
	LOG("Saving player coordinates");
	bool ret = true;

	pugi::xml_node player = save.append_child("coordinates");
	player.append_attribute("x").set_value(playerRect.x);
	player.append_attribute("y").set_value(playerRect.y);

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
	ListItem<MapLayer*>* mapLayer = app->map->data.mapLayer.start;
	SString layerName;
	if (isObject)
	{
		layerName = "Objects";
	}
	else
	{
		layerName = "Collisions";
	}
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
	SString tileSetName;
	if (notMovCollision)
	{
		tileSetName = "level_1_tileset";
	}
	else
	{
		tileSetName = "meta_data";
	}
	while (tileSet != NULL)
	{
		if (tileSet->data->name == tileSetName)
		{
			break;
		}
		tileSet = tileSet->next;
	}

	// Gets CollisionId
	int id = (int)(mapLayer->data->Get(x, y) - tileSet->data->firstgId);
	if (id < 0)
	{
		ret = 0;
		return ret;
	}
	Tile* currentTile = tileSet->data->GetPropList(id);
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