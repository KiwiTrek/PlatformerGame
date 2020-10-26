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

	//Collisions
	//ListItem <MapLayer*>* ML = app->map->data.mapLayer.start;
	//while (ML != NULL) {
	//	if (ML->data->name.GetString() == "Collisions") {
	//		break;
	//	}
	//	ML = ML->next;
	//}
	//ListItem <Tileset*>* T = app->map->data.tilesets.start;
	//while (T != NULL) {
	//	if (T->data->name.GetString() == "MetaData") {
	//		break;
	//	}
	//	T = T->next;
	//}

	//if(T->data-> ML->data->Get(0,0))



	if (isDead)
	{
		if (currentAnimation->HasFinished())
		{
			app->transition->FadeEffect((Module*)app->scene, (Module*)app->deathScene, false, 600.0f);
		}
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