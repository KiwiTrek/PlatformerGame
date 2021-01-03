#include "EntityManager.h"

#include "App.h"

#include "Collisions.h"
#include "Map.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "PathFinding.h"
#include "Transition.h"
#include "Fonts.h"
#include "GuiManager.h"

#include "Player.h"
#include "EnemyFlying.h"
#include "EnemyGround.h"
#include "Coin.h"

#include "Scene.h"

#include "Defs.h"
#include "Log.h"

#define TITLE_FONT_SIZE 36

EntityManager::EntityManager() : Module()
{
	name.Create("entityManager");
}

EntityManager::~EntityManager()
{}

void EntityManager::Init()
{
	active = false;
}

bool EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Loading Entity Manager");
	bool ret = true;

	folderAudioFx.Create(config.child("folderAudioFx").child_value());
	folderTexture.Create(config.child("folderTexture").child_value());
	folderMap.Create(config.child("folderMap").child_value());

	return ret;
}

bool EntityManager::Start()
{
	// Fx
	SString tmp("%s%s", folderAudioFx.GetString(), "enemy_death.wav");
	enemyDestroyedFx = app->audio->LoadFx(tmp.GetString());

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

	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "enemy_death.wav");
	enemyDestroyedFx = app->audio->LoadFx(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderAudioFx.GetString(), "coin.wav");
	coinFx = app->audio->LoadFx(tmp.GetString());

	app->audio->SetFxVolume(deadFx);
	app->audio->SetFxVolume(jumpFx);
	app->audio->SetFxVolume(doubleJumpFx);
	app->audio->SetFxVolume(fruitFx);
	app->audio->SetFxVolume(hitFx);
	app->audio->SetFxVolume(slashFx);
	app->audio->SetFxVolume(checkpointFx);
	app->audio->SetFxVolume(enemyDestroyedFx);
	app->audio->SetFxVolume(coinFx);

	// Tex
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "character_spritesheet.png");
	playerTex = app->tex->Load(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "heart_animation.png");
	playerHeart = app->tex->Load(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "enemy_ground_spritesheet.png");
	ground = app->tex->Load(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "enemy_flying_spritesheet.png");
	flying = app->tex->Load(tmp.GetString());

	tmp.Clear();
	tmp.Create("%s%s", folderMap.GetString(), "level_1_tileset.png");
	coin = app->tex->Load(tmp.GetString());

	// Pause Menu
	cameraPos = { -app->render->camera.x, -app->render->camera.y };
	cameraSize = { app->render->camera.w, app->render->camera.h };

	pauseFont = app->gui->titleFont;
	pauseTitle.Create("- PAUSE SCREEN -");
	offsetTitle = pauseTitle.Length() * TITLE_FONT_SIZE;
	btnResume = (GuiButton*)app->gui->CreateGuiControl(GuiControlType::BUTTON, 1, { cameraPos.x + (cameraSize.x / 2), 150, 217, 109 }, "RESUME", this);
	btnSettings = (GuiButton*)app->gui->CreateGuiControl(GuiControlType::BUTTON, 2, { cameraPos.x + (cameraSize.x / 2), 260, 217, 109 }, "SETTINGS", this);
	btnTitle = (GuiButton*)app->gui->CreateGuiControl(GuiControlType::BUTTON, 3, { cameraPos.x + (cameraSize.x / 2), 370, 217, 109 }, "BACK to", this, 0, true, "TITLE");
	btnExit = (GuiButton*)app->gui->CreateGuiControl(GuiControlType::BUTTON, 4, { cameraPos.x + (cameraSize.x / 2), 480, 217, 109 }, "EXIT", this);
	exitRequest = false;

	// Settings menu
	settings = false;
	settingsTitle.Create("- SETTINGS MENU -");
	offsetSettings = settingsTitle.Length() * TITLE_FONT_SIZE;
	sldrMusic = (GuiSlider*)app->gui->CreateGuiControl(GuiControlType::SLIDER, 91, { cameraPos.x + (cameraSize.x / 2) + 84, 150, 54, 54 }, "Music Volume", this, 10);
	sldrFx = (GuiSlider*)app->gui->CreateGuiControl(GuiControlType::SLIDER, 92, { cameraPos.x + (cameraSize.x / 2) + 84, 260, 54, 54 }, "Music Volume", this, 10);
	chckFullscreen = (GuiCheckBox*)app->gui->CreateGuiControl(GuiControlType::CHECKBOX, 93, { cameraPos.x + (cameraSize.x / 2) + 84,370,54,54 }, "Fullscreen", this);
	chckVSync = (GuiCheckBox*)app->gui->CreateGuiControl(GuiControlType::CHECKBOX, 94, { cameraPos.x + (cameraSize.x / 2) + 184,370,54,54 }, "VSync", this);

	btnBack = (GuiButton*)app->gui->CreateGuiControl(GuiControlType::BUTTON, 6, { cameraPos.x + 976, cameraPos.x + 553, 217, 109 }, "BACK", this);

	doLogic = true;

	return true;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	ListItem<Entity*>* e = entities.start;
	while (e != nullptr)
	{
		ListItem<Entity*>* eNext = e->next;
		DestroyEntity(e->data);
		e = eNext;
	}

	app->tex->UnLoad(playerTex);
	app->tex->UnLoad(ground);
	app->tex->UnLoad(flying);
	app->tex->UnLoad(coin);

	app->audio->UnloadFx(deadFx);
	app->audio->UnloadFx(doubleJumpFx);
	app->audio->UnloadFx(fruitFx);
	app->audio->UnloadFx(jumpFx);
	app->audio->UnloadFx(hitFx);
	app->audio->UnloadFx(slashFx);
	app->audio->UnloadFx(checkpointFx);
	app->audio->UnloadFx(enemyDestroyedFx);
	app->audio->UnloadFx(coinFx);

	app->gui->DestroyGuiControl(btnResume);
	btnResume = nullptr;
	app->gui->DestroyGuiControl(btnSettings);
	btnSettings = nullptr;
	app->gui->DestroyGuiControl(btnTitle);
	btnTitle = nullptr;
	app->gui->DestroyGuiControl(btnExit);
	btnExit = nullptr;

	return true;
}

Entity* EntityManager::CreateEntity(int x, int y, EntityType type, Entity* playerPointer, EnemyType eType)
{
	Entity* ret = nullptr;

	switch (type)
	{
		// L13: Create the corresponding type entity
	case EntityType::PLAYER:
	{
		ret = new Player(x, y);
		break;
	}
	case EntityType::ENEMY:
	{
		switch (eType)
		{
		case EnemyType::FLYING:
		{
			ret = new EnemyFlying(x, y, eType, playerPointer);
			break;
		}
		case EnemyType::GROUND:
		{
			ret = new EnemyGround(x, y, eType, playerPointer);
			break;
		}
		default:
		{
			break;
		}
		}
		break;
	}
	case EntityType::COIN:
	{
		ret = new Coin(x, y);
	}
	default:
	{
		break;
	}
	}

	// Created entities are added to the list
	if (ret != nullptr) entities.Add(ret);

	return ret;
}

bool EntityManager::Update(float dt)
{
	cameraPos = { -app->render->camera.x, -app->render->camera.y };

	dtTmp = dt;
	if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
	{
		app->CapRequest();
	}

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		doLogic = false;
		btnResume->bounds.x = (cameraSize.x / 2) - (btnResume->bounds.w / 2);
		btnResume->bounds.y = 175;

		btnSettings->bounds.x = (cameraSize.x / 2) - (btnSettings->bounds.w / 2);
		btnSettings->bounds.y = 300;

		btnTitle->bounds.x = (cameraSize.x / 2) - (btnTitle->bounds.w / 2);
		btnTitle->bounds.y = 425;

		btnExit->bounds.x = (cameraSize.x / 2) - (btnExit->bounds.w / 2);
		btnExit->bounds.y = 550;
	}

	UpdateAll(dt, doLogic);

	if (!app->entities->doLogic)
	{
		app->entities->doLogic = false;
		btnResume->Update(dt);
		btnSettings->Update(dt);
		btnTitle->Update(dt);
		btnExit->Update(dt);
	}

	return true;
}

bool EntityManager::UpdateAll(float dt, bool doLogic)
{
	if (doLogic)
	{
		app->collisions->PreUpdate();
		ListItem<Entity*>* e = entities.start;
		while (e != nullptr)
		{
			e->data->Update(dt);
			//if (e->data->currentAnim != nullptr)
			//{
			//	e->data->currentAnim->Update(dt);
			//}
			//e->data->physics.UpdatePhysics(e->data->nextPos, dt);
			//e->data->physics.ResolveCollisions(e->data->entityRect, e->data->nextPos, e->data->invert);
			//if (e->data->collider != nullptr)
			//{
			//	e->data->collider->SetPos(e->data->entityRect.x, e->data->entityRect.y, e->data->currentAnim->GetCurrentFrame().w, e->data->currentAnim->GetCurrentFrame().h);
			//}
			//e->data->PostUpdate(dt);
			e = e->next;
		}
	}

	return true;
}

bool EntityManager::PostUpdate()
{
	ListItem<Entity*>* e = entities.start;
	while (e != nullptr)
	{
		if (e->data->currentAnim != nullptr)
		{
			e->data->Draw();
		}
		if (e->data->pendingToDelete == true)
		{
			DestroyEntity(e->data);
		}
		e = e->next;
	}

	if (!app->entities->doLogic)
	{
		app->render->DrawRectangle({ cameraPos.x,cameraPos.y,cameraSize.x,cameraSize.y }, 0, 0, 0, 191);
		app->fonts->DrawText(cameraPos.x + (cameraSize.x - offsetTitle) / 2,cameraPos.y + 100, pauseFont, pauseTitle.GetString());
		btnResume->Draw(cameraPos.x,cameraPos.y);
		btnSettings->Draw(cameraPos.x, cameraPos.y);
		btnTitle->Draw(cameraPos.x, cameraPos.y);
		btnExit->Draw(cameraPos.x, cameraPos.y);
	}

	return true;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	entity->collider->pendingToDelete = true;
	int i = entities.Find(entity);
	delete entities[i];
	entities.Del(entities.At(i));
}

void EntityManager::OnCollision(Collider* c1, Collider* c2)
{
	for (int i = 0; i < entities.Count(); i++)
	{
		if (c1 == entities[i]->collider)
		{
			entities[i]->OnCollision(c1, c2);
		}
	}
}

bool EntityManager::OnGuiMouseClickEvent(GuiControl* control)
{
	switch (control->type)
	{
	case GuiControlType::BUTTON:
	{
		switch (control->id)
		{
		case 1:	//Resume
		{
			doLogic = false;
			app->entities->doLogic = true;
			break;
		}
		case 2: //Settings
		{
			break;
		}
		case 3:	//Title
		{
			app->transition->FadeEffect((Module*)app->scene, (Module*)app->titleScene, false, floor(1200.0f * dtTmp));
			break;
		}
		case 4: //Exit
		{
			exitRequest = true;
			break;
		}
		default:
		{
			break;
		}
		}
	}
	default: break;
	}

	return true;
}

//bool EnemyManagement::Load(pugi::xml_node& save)
//{
//	LOG("Loading enemy data");
//	bool ret = true;
//	int i = 0;
//
//	int x = 0;
//	int y = 0;
//	EnemyType type = EnemyType::NO_TYPE;
//	bool destroyed = false;
//
//	for (pugi::xml_node enemy = save.child("enemy"); enemy && ret; enemy = enemy.next_sibling("enemy"))
//	{
//		destroyed = enemy.child("destroyed").attribute("value").as_bool();
//		if (destroyed == true && enemies[i] != nullptr)
//		{
//			enemies[i]->pendingToDelete = true;
//		}
//		else
//		{
//			x = enemy.child("coordinates").attribute("x").as_int();
//			y = enemy.child("coordinates").attribute("y").as_int();
//			switch (enemy.child("type").attribute("value").as_int())
//			{
//			case 1:
//				type = EnemyType::GROUND;
//				break;
//			case 2:
//				type = EnemyType::FLYING;
//				break;
//			default:
//				type = EnemyType::NO_TYPE;
//				break;
//			}
//
//			if (enemies[i] == nullptr)
//			{
//				AddEnemy(type, x, y, i);
//			}
//			else
//			{
//				enemies[i]->enemyRect.x = x;
//				enemies[i]->enemyRect.y = y;
//			}
//		}
//		i++;
//	}
//
//	return ret;
//}
//
//bool EnemyManagement::Save(pugi::xml_node& save)
//{
//	LOG("Saving enemy data");
//	bool ret = true;
//
//	for (uint i = 0; i < MAX_ENEMIES; ++i)
//	{
//		pugi::xml_node enemy = save.append_child("enemy");
//		if (enemies[i] != nullptr)
//		{
//			pugi::xml_node enemyCoords = enemy.append_child("coordinates");
//			enemyCoords.append_attribute("x").set_value(enemies[i]->enemyRect.x);
//			enemyCoords.append_attribute("y").set_value(enemies[i]->enemyRect.y);
//			enemy.append_child("type").append_attribute("value").set_value(enemies[i]->type);
//			enemy.append_child("destroyed").append_attribute("value").set_value(enemies[i]->pendingToDelete);
//		}
//		else
//		{
//			enemy.append_child("destroyed").append_attribute("value").set_value(true);
//		}
//	}
//	return ret;
//}