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
	jumpCounter = 2;

	godMode = false;
	keyPressed = false;
	isJumping = false;
	isDead = false;
	invert = false;

	onceDead = true;

	playerPhysics.axisY = true;

	playerTex = app->tex->Load("Assets/textures/characterSpritesheet.png");
	deadFx = app->audio->LoadFx("Assets/audio/fx/lose.wav");
	jumpFx = app->audio->LoadFx("Assets/audio/fx/jump.wav");
	doubleJumpFx = app->audio->LoadFx("Assets/audio/fx/doubleJump.wav");
	app->audio->SetFxVolume(deadFx);
	app->audio->SetFxVolume(jumpFx);
	app->audio->SetFxVolume(doubleJumpFx);
    return true;
}

bool Player::Awake(pugi::xml_node&)
{
	for (int i = 0; i != 9; ++i)
	{
		idle.PushBack({ 10 + (playerSize * i),1329,56,73 });
	}
	idle.speed = 0.01f;
	idle.loop = true;

	for (int i = 0; i != 8; ++i)
	{
		run.PushBack({ 10 + (playerSize * i),1202,62,82 });
	}
	run.speed = 0.01f;
	run.loop = true;

	for (int i = 0; i != 2; ++i)
	{
		jumpPrep.PushBack({ 10 + (playerSize * i), 812, 60, 90 });
	}
	jumpPrep.speed = 0.05f;
	jumpPrep.loop = false;

	for (int i = 0; i != 4; ++i)
	{
		jumpMid.PushBack({ 10 + (2 * playerSize) + (playerSize * i),812,60,80 });
	}
	jumpMid.speed = 0.01f;
	jumpMid.loop = true;

	jumpLand.PushBack({ 10 + (playerSize * 6), 818, 60, 72 });
	jumpLand.PushBack({ 10 + (playerSize * 6), 818, 60, 72 });
	jumpLand.speed = 0.3f;
	jumpLand.loop = false;

	for (int i = 0; i != 5; ++i)
	{
		death.PushBack({ 10 + (playerSize * i),192,88,66 });
	}
	death.speed = 0.03f;
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

	if (isDead == false)
	{
		if (godMode)		//4 directional movement
		{
			if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			{
				playerRect.y -= 1;
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			{
				playerRect.y += 1;
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT)
			{
				playerRect.x -= 1;
				currentAnimation = &run;
				if (invert == false)
				{
					invert = true;
				}
				keyPressed = true;
			}
			if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT)
			{
				playerRect.x += 1;
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
			if (!isJumping || jumpCounter > 0)
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
					speed.y = -100.0f;
				}
			}
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT)
			{
				playerRect.x -= 1;
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
				playerRect.x += 1;
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
	}

	//Physics
	playerPhysics.UpdatePhysics(playerRect.y, speed.y);

	//Collisions
	int x = playerRect.x / 64;
	int resX = playerRect.x % 64;
	int y = playerRect.y / 64;
	int resY = playerRect.y % 64;
	if (x < 0) {
		x = 0;
	}
	if (y < 0) {
		y = 0;
	}

	//if (y == 11) { // to enter the GetColliderId on its first iteration with the floor thanks to a breakpoint
	//	LOG("b");
	//}

	if (GetColliderId(x, y) == Collider::TYPE::SOLID					//Ceiling x+-1,y,y+1
		&& GetColliderId(x, y + 1) == Collider::TYPE::AIR
		&& (GetColliderId(x - 1, y) == Collider::TYPE::SOLID
		|| GetColliderId(x + 1, y) == Collider::TYPE::SOLID
		|| GetColliderId(x, y + 2) == Collider::TYPE::SOLID))
	{
		playerRect.y = (y + 1) * 2 * 64 - (playerRect.y);
		speed.y = 0;
		LOG("Ceiling!");
	}
	else if (GetColliderId(x, y + 1) == Collider::TYPE::SOLID			//Floor x+-1,y,y-1
		&& GetColliderId(x, y) == Collider::TYPE::AIR
		&& (GetColliderId(x - 1, y + 1) == Collider::TYPE::SOLID
		|| GetColliderId(x + 1, y + 1) == Collider::TYPE::SOLID
		|| GetColliderId(x, y + 2) == Collider::TYPE::SOLID))
	{
		//reset jump
		playerRect.y = (y) * 2 * 64 - (playerRect.y);
		speed.y = 0;
		LOG("Floor!");
		if (isJumping)
		{
			currentAnimation = &jumpLand;
		}
	}


	if (GetColliderId(x + 1, y) == Collider::TYPE::SOLID				//Right Wall x,x+1,y+-1
		&& GetColliderId(x, y) == Collider::TYPE::AIR
		&& (GetColliderId(x, y - 1) == Collider::TYPE::SOLID
		|| GetColliderId(x, y + 1) == Collider::TYPE::SOLID
		|| GetColliderId(x + 2, y) == Collider::TYPE::SOLID))
	{
		//reset jump
		playerRect.x = 2 * 64 * (x + 1) - 64 * 2 - playerRect.x;
		LOG("Right Wall!");
	}
	else if (GetColliderId(x, y) == Collider::TYPE::SOLID				//Left Wall x,x-1,y+-1
		&& GetColliderId(x + 1, y) == Collider::TYPE::AIR
		&& (GetColliderId(x, y - 1) == Collider::TYPE::SOLID
		|| GetColliderId(x, y + 1) == Collider::TYPE::SOLID
		|| GetColliderId(x - 1, y) == Collider::TYPE::SOLID))
	{
		//reset jump
		playerRect.x = 2 * 64 * (x + 1) - playerRect.x;
		LOG("Left Wall!");
	}


	// Spawns
	//if (GetColliderId(x, y) == Collider::TYPE::SPAWN)
	//{
	//	spawnPoint = { x, y };
	//}

	// Dead
	//if (GetColliderId(x, y) == Collider::TYPE::SPIKE) {
	//	isDead = true;
	//}
	if (isDead)
	{
		currentAnimation = &death;
		if (onceDead)
		{
			onceDead = false;
			app->audio->PlayFx(deadFx);
		}
		if (currentAnimation->HasFinished())
		{
			app->transition->FadeEffect((Module*)app->scene, (Module*)app->deathScene, false, 600.0f);
		}
	}

	// Win condition
	//if (GetColliderId(x, y) == Collider::TYPE::GOAL)
	//{
	//	LOG("YOU WIN, YAY!\n");
	//}

    return true;
}

bool Player::PostUpdate()
{
	if (playerRect.x <= 0)
	{
		playerRect.x = 0;
	}
	if ((playerRect.x + playerRect.w) > (app->map->data.w * app->map->data.tileW))
	{
		--playerRect.x;
	}

	//app->render->DrawRectangle({ playerRect.x, playerRect.y + 64, 64, 64 }, 255, 0, 0, 255); // temp
	//app->render->DrawRectangle({ playerRect.x, playerRect.y - 64, 64, 64 }, 0, 255, 0, 255); // temp
	app->render->DrawTexture(playerTex, playerRect.x, playerRect.y, false, &currentAnimation->GetCurrentFrame(), invert);
    return true;
}

bool Player::CleanUp()
{
	app->tex->UnLoad(playerTex);

	app->audio->UnloadFx(deadFx);

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

int Player::GetColliderId(int x, int y, bool isFruit, bool isObject) const
{
	int ret;
	// MapLayer		<- this works
	ListItem <MapLayer*>* ML = app->map->data.mapLayer.start;
	SString layerName;
	if (isObject) {
		layerName = "Objects";
	}
	else {
		layerName = "Collisions";
	}
	while (ML != NULL) {
		if (ML->data->name == layerName) {
			break;
		}
		ML = ML->next;
	}

	// Tileset		<- this works
	ListItem <Tileset*>* T = app->map->data.tilesets.start;
	SString tilesetName;
	if (isFruit) {
		tilesetName = "Level1Tileset(64x64)";
	}
	else {
		tilesetName = "MetaData";
	}
	while (T != NULL) {
		if (T->data->name == tilesetName) {
			break;
		}
		T = T->next;
	}

	// Gets CollisionId
	int id = (int)(ML->data->Get(x, y) - T->data->firstgId);	//returns id of the tile
	if (id < 0) {
		ret = 0;
		return ret;
	}
	Tile* currentTile = T->data->GetPropList(id);					//on second iteration there is no properties list (we think it gets destroyed)
	ret = currentTile->properties.GetProperty("CollisionId",0);						//since there is no getpropList it triggers breakpoint and explodes
	//LOG("%d - %d", id, ret);
	return ret;
}