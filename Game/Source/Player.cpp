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
	godMode = true;
	keyPressed = false;
	isDead = false;
	invert = false;

	playerPhysics.axisX = false;
	playerPhysics.axisY = false;

	playerTex = app->tex->Load("Assets/textures/characterSpritesheet.png");
	deadFx = app->audio->LoadFx("Assets/audio/fx/lose.wav");
    return true;
}

bool Player::Awake(pugi::xml_node&)
{
	for (int i = 0; i != 9; ++i)
	{
		idle.PushBack({ 22 + (playerSize * i),1330,44,72 });
	}
	idle.SetSpeed(0.05f);
	idle.SetLoop(true);

	for (int i = 0; i != 8; ++i)
	{
		run.PushBack({ 22 + (playerSize * i),1202,50,75 });
	}
	run.SetSpeed(0.05f);
	run.SetLoop(true);

/*for (int i = 0; i != 7; ++i)
{
	jump.PushBack({ 22 + (playerSize * i),820,49,80 });
}
jump.SetSpeed(0.1f);
jump.SetLoop(false);*/

for (int i = 0; i != 5; ++i)
{
	death.PushBack({ 22 + (playerSize * i),184,76,66 });
}
death.SetSpeed(0.03f);
death.SetLoop(false);

wallJump.PushBack({ 648,170,55,79 });
wallJump.SetSpeed(0.0f);
wallJump.SetLoop(false);

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
		currentAnimation = &death;
		isDead = true;
		app->audio->PlayFx(deadFx);
	}

	if (isDead == false)
	{
		if (godMode)
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
			/*if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
			{
				currentAnimation = &jump;
				keyPressed = true;
			}*/
		}

		if (keyPressed == false)
		{
			run.Reset();
			//jump.Reset();
			currentAnimation = &idle;
		}
	}

	//Physics
	playerPhysics.UpdatePhysics(playerRect.x, playerRect.y, speed.x, speed.y);

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
	
	//Left Wall x,x-1,y+-1
	//if (GetColliderId(x, y) == Collider::TYPE::SOLID
	//	&& (GetColliderId(x, y) == Collider::TYPE::AIR
	//	|| GetColliderId(x, y) == Collider::TYPE::WATER)
	//	&& ((GetColliderId(x, y - 1) == Collider::TYPE::SOLID
	//	&& (GetColliderId(x - 1, y - 1) == Collider::TYPE::AIR
	//	|| GetColliderId(x - 1, y - 1) == Collider::TYPE::WATER))
	//	|| (GetColliderId(x, y + 1) == Collider::TYPE::SOLID
	//	&& (GetColliderId(x - 1, y + 1) == Collider::TYPE::AIR
	//	|| GetColliderId(x - 1, y + 1) == Collider::TYPE::WATER))))
	//{
	//	//reset jump
	//	playerRect.x = 2 * 64 * (x - 1) - playerRect.x;
	//	//LOG("%d - LEFT WALL!", playerRect.x);
	//}
	//else
	//{
	//	//LOG("NOT LEFT WALL!");
	//}

	//Right Wall x,x+1,y+-1
	//if (GetColliderId(x, y) == Collider::TYPE::SOLID
	//	&& (GetColliderId(x + 1, y) == Collider::TYPE::AIR
	//	|| GetColliderId(x + 1, y) == Collider::TYPE::WATER)
	//	&& ((GetColliderId(x, y - 1) == Collider::TYPE::SOLID
	//	&& (GetColliderId(x + 1, y - 1) == Collider::TYPE::AIR
	//	|| GetColliderId(x + 1, y - 1) == Collider::TYPE::WATER))
	//	|| (GetColliderId(x, y + 1) == Collider::TYPE::SOLID
	//	&& (GetColliderId(x + 1, y + 1) == Collider::TYPE::AIR
	//	|| GetColliderId(x + 1, y + 1) == Collider::TYPE::WATER))))
	//{
	//	//reset jump
	//	playerRect.x = 2 * 64 * (x + 1) - 64 * 2 - playerRect.x;
	//	//LOG("RIGHT WALL!");
	//}
	//else
	//{
	//	//LOG("NOT RIGHT WALL!");
	//}

	//Floor x+-1,y,y+1
	if (GetColliderId(x, y) == Collider::TYPE::SOLID
		&& GetColliderId(x, y - 1) == Collider::TYPE::AIR
		&& (GetColliderId(x - 1, y) == Collider::TYPE::SOLID
		|| GetColliderId(x + 1, y) == Collider::TYPE::SOLID))
	{
		//reset jump
		playerRect.y = y * 64;
		speed.y = 0;
		//LOG("FLOOR!");
	}
	else
	{
		//LOG("NOT FLOOR!");
	}

	//Ceiling x+-1,y,y-1
	//if (GetColliderId(x, y) == Collider::TYPE::SOLID
	//	&& (GetColliderId(x, y - 1) == Collider::TYPE::AIR
	//	|| GetColliderId(x, y - 1) == Collider::TYPE::WATER)
	//	&& ((GetColliderId(x - 1, y) == Collider::TYPE::SOLID
	//	&& (GetColliderId(x - 1, y - 1) == Collider::TYPE::AIR
	//	|| GetColliderId(x - 1, y - 1) == Collider::TYPE::WATER))
	//	|| (GetColliderId(x + 1, y) == Collider::TYPE::SOLID
	//	&& (GetColliderId(x + 1, y - 1) == Collider::TYPE::AIR
	//	|| GetColliderId(x + 1, y - 1) == Collider::TYPE::WATER))))
	//{
	//	playerRect.y = (y + 2) * 64 - 64;
	//	speed.y = 0;
	//	//LOG("CEILING!");
	//}
	//else
	//{
	//	//LOG("NOT CEILING!");
	//}
	

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

	app->render->DrawRectangle({ playerRect.x, playerRect.y + 64, 64, 64 }, 255, 0, 0, 255); // temp
	app->render->DrawRectangle({ playerRect.x, playerRect.y - 64, 64, 64 }, 0, 255, 0, 255); // temp
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