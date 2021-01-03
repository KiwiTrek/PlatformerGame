#include "DeathScene.h"

#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "TitleScene.h"
#include "Transition.h"
#include "Scene.h"
#include "GuiManager.h"

#include "Defs.h"
#include "Log.h"


DeathScene::DeathScene() : Module()
{
	name.Create("deathScene");
}

DeathScene::~DeathScene()
{}

void DeathScene::Init()
{
	active = false;
}

bool DeathScene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	folderTexture.Create(config.child("folderTexture").child_value());
	folderAudioMusic.Create(config.child("folderAudioMusic").child_value());

	return ret;
}

bool DeathScene::Start()
{
	app->render->SetBackgroundColor({ 0,0,0,0 });
	SString tmp("%s%s", folderTexture.GetString(), "game_over.png");
	deathScreen = app->tex->Load(tmp.GetString());
	tmp.Clear();
	tmp.Create("%s%s", folderAudioMusic.GetString(), "game_over.ogg");
	app->audio->PlayMusic(tmp.GetString(), 0.5f);

	state = 0;
	timer = 0.0f;
	alpha = 0.0f;

	app->gui->Enable();

	btnTitle = (GuiButton*)app->gui->CreateGuiControl(GuiControlType::BUTTON, 1, { -app->render->camera.x + 193, -app->render->camera.y + 498, 217, 109 }, "BACK to", this, 0, true, "TITLE");
	btnExit = (GuiButton*)app->gui->CreateGuiControl(GuiControlType::BUTTON, 2, { -app->render->camera.x + 867, -app->render->camera.y + 498, 217, 109 }, "EXIT", this);

	return true;
}

bool DeathScene::PreUpdate()
{
	return true;
}

bool DeathScene::Update(float dt)
{
	dtTmp = dt;
	if (state == 0)
	{
		state = 1;
	}
	else if (state == 1)
	{
		timer += dt;
		if (timer >= 5.0f)
		{
			state = 2;
		}
	}
	else if (state == 2)
	{
		alpha += (1.0f * dt);

		if (alpha > 1.0f)
		{
			alpha = 1.0f;
			state = 3;
		}
	}
	else if (state == 3)
	{
		btnTitle->Update(dt);
		btnExit->Update(dt);
	}

	return true;
}

bool DeathScene::PostUpdate()
{
	bool ret = true;

	if (exitRequest)
	{
		ret = false;
	}

	app->render->DrawTexture(deathScreen, NULL, NULL, true);

	btnTitle->Draw(-app->render->camera.x, -app->render->camera.y);
	btnExit->Draw(-app->render->camera.x, -app->render->camera.y);
	app->render->DrawRectangle(btnTitle->bounds, 0, 0, 0, (uchar)(255 - (255 * alpha)));
	app->render->DrawRectangle(btnExit->bounds, 0, 0, 0, (uchar)(255 - (255 * alpha)));

	return ret;
}

bool DeathScene::CleanUp()
{
	LOG("Freeing scene");

	app->tex->UnLoad(deathScreen);

	app->gui->DestroyGuiControl(btnTitle);
	btnTitle = nullptr;
	app->gui->DestroyGuiControl(btnExit);
	btnExit = nullptr;

	app->gui->Disable();

	return true;
}

bool DeathScene::OnGuiMouseClickEvent(GuiControl* control)
{
	switch (control->type)
	{
	case GuiControlType::BUTTON:
	{
		switch (control->id)
		{
		case 1:	//Back to title
		{
			app->transition->FadeEffect(this, (Module*)app->titleScene, false, floor(1200.0f * dtTmp));
			break;
		}
		case 2:	//Exit
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