#include "Player.h"

#include "App.h"

#include "Textures.h"
#include "Input.h"
#include "Audio.h"
#include "Render.h"
#include "Scene.h"
#include "Map.h"

#include "Log.h"

Player::Player() {}

Player::~Player() {}

void Player::Init()
{
	active = false;
}

bool Player::Start()
{
	playerTex = app->tex->Load("Assets/textures/characterSpritesheet.png");
    return true;
}

bool Player::Awake(pugi::xml_node&)
{
	playerRect = { 64 * 3,64 * 8,22,36 };
	playerIdle = { 12,25,22,36 };

    return true;
}

bool Player::PreUpdate()
{
    return true;
}

bool Player::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		playerRect.y -= 1;
	}
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		playerRect.y += 1;
	}
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		playerRect.x -= 1;
	}
	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		playerRect.x += 1;
	}
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

	app->render->DrawTexture(playerTex, playerRect.x, playerRect.y, false, &playerIdle);
    return true;
}

bool Player::CleanUp()
{
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