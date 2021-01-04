#include "EntityManager.h"

#include "App.h"

#include "Window.h"
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
#include "Fruit.h"

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

	tmp.Clear();
	tmp.Create("%s%s", folderMap.GetString(), "level_1_tileset.png");
	fruit = app->tex->Load(tmp.GetString());

	app->gui->Enable();
	// Pause Menu
	cameraPos = { -app->render->camera.x, -app->render->camera.y };
	cameraSize = { app->render->camera.w, app->render->camera.h };

	pauseFont = app->gui->titleFont;
	pauseTitle.Create("- PAUSE SCREEN -");
	offsetTitle = pauseTitle.Length() * TITLE_FONT_SIZE;
	btnResume = (GuiButton*)app->gui->CreateGuiControl(GuiControlType::BUTTON, 1, { (cameraSize.x / 2) - (217 / 2), 175, 217, 109 }, "RESUME", this);
	btnSettings = (GuiButton*)app->gui->CreateGuiControl(GuiControlType::BUTTON, 2, { (cameraSize.x / 2) - (217 / 2), 300, 217, 109 }, "SETTINGS", this);
	btnTitle = (GuiButton*)app->gui->CreateGuiControl(GuiControlType::BUTTON, 3, { (cameraSize.x / 2) - (217 / 2), 425, 217, 109 }, "BACK to", this, 0, true, "TITLE");
	btnExit = (GuiButton*)app->gui->CreateGuiControl(GuiControlType::BUTTON, 4, { (cameraSize.x / 2) - (217 / 2), 550, 217, 109 }, "EXIT", this);
	exitRequest = false;

	// Settings menu
	float tmpValue = 0;
	settings = false;
	settingsTitle.Create("- SETTINGS -");
	offsetSettings = settingsTitle.Length() * 36;
	sldrMusic = (GuiSlider*)app->gui->CreateGuiControl(GuiControlType::SLIDER, 101, { (1280 / 4) + 132, 150, 54, 54 }, "Music Volume", this, 10);
	sldrMusic->value = app->audio->GetMusicVolume();
	sldrMusic->maxValue = 128;
	tmpValue = (float)(sldrMusic->limits.w - sldrMusic->bounds.w) / (float)sldrMusic->maxValue;
	sldrMusic->bounds.x = sldrMusic->limits.x + (sldrMusic->value * tmpValue);
	sldrFx = (GuiSlider*)app->gui->CreateGuiControl(GuiControlType::SLIDER, 102, { (1280 / 4) + 132, 260, 54, 54 }, "SFX Volume", this, 10);
	sldrFx->value = app->audio->GetFxVolume();
	sldrFx->maxValue = 128;
	tmpValue = (float)(sldrFx->limits.w - sldrFx->bounds.w) / (float)sldrFx->maxValue;
	sldrFx->bounds.x = sldrFx->limits.x + (tmpValue * sldrFx->value);
	chckFullscreen = (GuiCheckBox*)app->gui->CreateGuiControl(GuiControlType::CHECKBOX, 103, { (1280 / 4) + 132,380,54,54 }, "Fullscreen", this);
	chckFullscreen->checked = app->win->fullscreenWindow;
	chckVsync = (GuiCheckBox*)app->gui->CreateGuiControl(GuiControlType::CHECKBOX, 104, { (1280 / 4) + 132,440,54,54 }, "VSync", this);
	chckVsync->checked = app->vsync;

	btnBack = (GuiButton*)app->gui->CreateGuiControl(GuiControlType::BUTTON, 105, { cameraPos.x + 976, cameraPos.x + 553, 217, 109 }, "BACK", this);

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

	app->gui->DestroyGuiControl(sldrMusic);
	sldrMusic = nullptr;
	app->gui->DestroyGuiControl(sldrFx);
	sldrFx = nullptr;
	app->gui->DestroyGuiControl(chckFullscreen);
	chckFullscreen = nullptr;
	app->gui->DestroyGuiControl(chckVsync);
	chckVsync = nullptr;
	app->gui->DestroyGuiControl(btnBack);
	btnBack = nullptr;

	app->gui->Disable();

	return true;
}

Entity* EntityManager::CreateEntity(int x, int y, EntityType type, Entity* playerPointer, EnemyType eType, int listNumber)
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
		break;
	}
	case EntityType::FRUIT:
	{
		ret = new Fruit(x, y);
		break;
	}
	default:
	{
		break;
	}
	}

	// Created entities are added to the list
	if (ret != nullptr)
	{
		switch (listNumber)
		{
		case 0:
			entities.Add(ret);
			break;
		case 1:
			loadingEntities.Add(ret);
			break;
		default:
			break;
		}
	}

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
	}

	UpdateAll(dt, doLogic);

	if (!app->entities->doLogic && !settings)
	{
		app->entities->doLogic = false;
		btnResume->Update(dt);
		btnSettings->Update(dt);
		btnTitle->Update(dt);
		btnExit->Update(dt);
	}
	
	if (settings)
	{
		sldrMusic->Update(dt);
		sldrFx->Update(dt);
		chckFullscreen->Update(dt);
		chckVsync->Update(dt);
		btnBack->Update(dt);
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
		//if (e->data->currentAnim != nullptr)
		//{
		//	e->data->Draw();
		//}
		if (e->data->pendingToDelete == true)
		{
			DestroyEntity(e->data);
		}
		else
		{
			e->data->Draw();
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

	if (settings)
	{
		app->render->DrawRectangle({ cameraPos.x,cameraPos.y,cameraSize.x,cameraSize.y }, 0, 0, 0, 191);
		offsetSettings = settingsTitle.Length() * TITLE_FONT_SIZE;
		app->fonts->DrawText(cameraPos.x + ((cameraSize.x - offsetSettings) / 2), cameraPos.y + 84, pauseFont, settingsTitle.GetString());
		sldrMusic->Draw(cameraPos.x, cameraPos.y);
		sldrFx->Draw(cameraPos.x, cameraPos.y);
		chckFullscreen->Draw(cameraPos.x, cameraPos.y);
		chckVsync->Draw(cameraPos.x, cameraPos.y);
		btnBack->Draw(cameraPos.x, cameraPos.y);
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
		settings = true;
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
	case 101: // MUSIC
	{
		app->audio->SetMusicVolume(sldrMusic->value);
		break;
	}
	case 102: // FX
	{
		app->audio->SetFxVolumeValue(sldrFx->value);
		break;
	}
	case 103: // FULLSCREEN
	{
		app->win->ToggleFullscreen(chckFullscreen->checked);
		break;
	}
	case 104: // VSYNC
	{
		app->render->ToggleVsync(chckVsync->checked, (Module*)app->scene);
		break;
	}
	case 105: // Back
	{
		settings = false;
	}
	default:
	{
		break;
	}
	}

	return true;
}

bool EntityManager::Load(pugi::xml_node& save)
{
	LOG("Loading entities data");
	bool ret = true;
	ListItem<Entity*>* e = entities.start;
	while (e != nullptr)
	{
		ListItem<Entity*>* eNext = e->next;
		DestroyEntity(e->data);
		e = eNext;
	}
	entities.Clear();

	int x = 0;
	int y = 0;
	EntityType type = EntityType::UNKNOWN;
	EnemyType eType = EnemyType::NO_TYPE;
	Player* pp = nullptr;

	for (pugi::xml_node entity = save.child("entity"); entity && ret; entity = entity.next_sibling("entity"))
	{
		x = entity.child("coordinates").attribute("x").as_int();
		y = entity.child("coordinates").attribute("y").as_int();
		switch (entity.child("type").attribute("value").as_int())
		{
		case 0:
			type = EntityType::PLAYER;
			break;
		case 1:
			type = EntityType::ENEMY;
			break;
		case 2:
			type = EntityType::COIN;
			break;
		case 3:
			type = EntityType::FRUIT;
			break;
		default:
			type = EntityType::UNKNOWN;
			break;
		}
		switch (entity.child("eType").attribute("value").as_int())
		{
		case 1:
			eType = EnemyType::GROUND;
			break;
		case 2:
			eType = EnemyType::FLYING;
			break;
		default:
			eType = EnemyType::NO_TYPE;
			break;
		}

		if (type == EntityType::PLAYER)
		{
			pp = (Player*)CreateEntity(x, y, type, nullptr, eType);
			pp->lives = entity.child("lives").attribute("value").as_int();
		}
		else
		{
			CreateEntity(x, y, type, pp, eType);
		}
	}

	//entities = loadingEntities;
	//loadingEntities.Clear();
	return ret;
}

bool EntityManager::Save(pugi::xml_node& save)
{
	LOG("Saving entities data");
	bool ret = true;

	//for (int i = 0; entities.At(i) != nullptr;)
	ListItem<Entity*>* e = entities.start;
	while (e != nullptr)
	{
		pugi::xml_node entity = save.append_child("entity");
		pugi::xml_node entityCoords = entity.append_child("coordinates");
		entityCoords.append_attribute("x").set_value(e->data->entityRect.x);
		entityCoords.append_attribute("y").set_value(e->data->entityRect.y);
		int type = 0;
		switch (e->data->type)
		{
		case EntityType::PLAYER:
		{
			Player* pp = (Player*)e->data;
			entity.append_child("lives").append_attribute("value").set_value(pp->lives);
			type = 0;
			break;
		}
		case EntityType::ENEMY:
		{
			type = 1;
			break;
		}
		case EntityType::COIN:
		{
			type = 2;
			break;
		}
		case EntityType::FRUIT:
		{
			type = 3;
			break;
		}
		default:
		{
			break;
		}
		}
		entity.append_child("type").append_attribute("value").set_value(type);
		int eType = 0;
		switch (e->data->eType)
		{
		case EnemyType::GROUND:
			eType = 1;
			break;
		case EnemyType::FLYING:
			eType = 2;
			break;
		default:
			break;
		}
		entity.append_child("eType").append_attribute("value").set_value(eType);

		//++i;
		e = e->next;
	}
	return ret;
}