#include "TitleScene.h"


#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Transition.h"
#include "Scene.h"
#include "Fonts.h"
#include "GuiManager.h"

#include "Defs.h"
#include "Log.h"

#define TITLE_FONT_SIZE 36
#define TITLE_FONT_SMALL_SIZE TITLE_FONT_SIZE/2

TitleScene::TitleScene() : Module()
{
	name.Create("titleScene");
}

TitleScene::~TitleScene()
{}

void TitleScene::Init()
{
	active = false;
}

bool TitleScene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;
	folderTexture.Create(config.child("folderTexture").child_value());
	folderAudioMusic.Create(config.child("folderAudioMusic").child_value());

	pugi::xml_node creditsNode = config.child("credits");
	title.Create(creditsNode.child("title").child_value());
	underLine.Create(creditsNode.child("underLine").child_value());
	authors.Create(creditsNode.child("authors").child_value());
	kiwi.Create(creditsNode.child("kiwi").child_value());
	lladruc.Create(creditsNode.child("lladruc").child_value());
	licence.Create(creditsNode.child("licences").child_value());
	licenceText1.Create(creditsNode.child("line1").child_value());
	licenceText2.Create(creditsNode.child("line2").child_value());
	licenceText3.Create(creditsNode.child("line3").child_value());
	licenceText4.Create(creditsNode.child("line4").child_value());
	licenceText5.Create(creditsNode.child("line5").child_value());
	licenceText6.Create(creditsNode.child("line6").child_value());

	return ret;
}

bool TitleScene::Start()
{
	app->render->SetBackgroundColor({ 0,0,0,0 });
	SString tmp("%s%s", folderTexture.GetString(), "title_screen.png");
	titleScreen = app->tex->Load(tmp.GetString());
	tmp.Clear();
	tmp.Create("%s%s", folderAudioMusic.GetString(), "title_screen.ogg");
	app->audio->PlayMusic(tmp.GetString(), 0.0f);

	exitRequest = false;

	credits = false;
	titleFont = app->gui->titleFont;
	titleFontSmall = app->gui->titleFontSmall;
	btnBack = (GuiButton*)app->gui->CreateGuiControl(GuiControlType::BUTTON, 6, { 976, 553, 217, 109 }, "BACK", this);

	btnPlay = (GuiButton*)app->gui->CreateGuiControl(GuiControlType::BUTTON, 1, { 84, 553, 217, 109 }, "PLAY", this);
	btnContinue = (GuiButton*)app->gui->CreateGuiControl(GuiControlType::BUTTON, 2, { 307, 553, 217, 109 }, "CONTINUE", this);
	btnSettings = (GuiButton*)app->gui->CreateGuiControl(GuiControlType::BUTTON, 3, { 530, 553, 217, 109 }, "SETTINGS", this);
	btnCredits = (GuiButton*)app->gui->CreateGuiControl(GuiControlType::BUTTON, 4, { 753, 553, 217, 109 }, "CREDITS", this);
	btnExit = (GuiButton*)app->gui->CreateGuiControl(GuiControlType::BUTTON, 5, { 976, 553, 217, 109 }, "EXIT", this);

	btnContinue->state = GuiControlState::DISABLED;

	//chckTest = (GuiCheckBox*)app->gui->CreateGuiControl(GuiControlType::CHECKBOX, 69, { (1280 / 2),10,54,54 }, "Test", this);
	//sldrTest = (GuiSlider*)app->gui->CreateGuiControl(GuiControlType::SLIDER, 420, { (1280 / 2), 25, 54, 54 }, "Another Test", this, 7);

	return true;
}

bool TitleScene::PreUpdate()
{
	return true;
}

bool TitleScene::Update(float dt)
{
	dtTmp = dt;

	if (!credits)
	{
		btnPlay->Update(dt);
		btnContinue->Update(dt);
		btnSettings->Update(dt);
		btnCredits->Update(dt);
		btnExit->Update(dt);
	}
	else
	{
		btnBack->Update(dt);
	}

	//chckTest->Update(dt);
	//sldrTest->Update(dt);

	return true;
}

bool TitleScene::PostUpdate()
{
	bool ret = true;

	if (exitRequest)
	{
		ret = false;
	}

	app->render->DrawTexture(titleScreen, NULL, NULL, true);

	if (!credits)
	{
		btnPlay->Draw();
		btnContinue->Draw();
		btnSettings->Draw();
		btnCredits->Draw();
		btnExit->Draw();
	}
	else
	{
		uint w, h;
		app->win->GetWindowSize(w, h);
		int width = (int)w;
		int height = (int)h;

		app->render->DrawRectangle({ 0,0,width,height }, 0, 0, 0, 191);
		offsetText = title.Length() * TITLE_FONT_SIZE;
		app->fonts->DrawText((w - offsetText) / 2, 84, titleFont, title.GetString());
		offsetText = underLine.Length() * TITLE_FONT_SIZE;
		app->fonts->DrawText((w - offsetText) / 2, 84 + TITLE_FONT_SMALL_SIZE, titleFont, underLine.GetString());
		app->fonts->DrawText(84, 84 + TITLE_FONT_SIZE * 2, titleFont, authors.GetString());
		app->fonts->DrawText(84, 84 + TITLE_FONT_SIZE * 3 + TITLE_FONT_SMALL_SIZE, titleFontSmall, kiwi.GetString());
		app->fonts->DrawText(84, 84 + TITLE_FONT_SIZE * 4 + TITLE_FONT_SMALL_SIZE, titleFontSmall, lladruc.GetString());
		app->fonts->DrawText(84, 84 + TITLE_FONT_SIZE * 6, titleFont, licence.GetString());
		app->fonts->DrawText(84, 84 + TITLE_FONT_SIZE * 7 + TITLE_FONT_SMALL_SIZE, titleFontSmall, licenceText1.GetString());
		app->fonts->DrawText(84, 84 + TITLE_FONT_SIZE * 8 + TITLE_FONT_SMALL_SIZE, titleFontSmall, licenceText2.GetString());
		app->fonts->DrawText(84, 84 + TITLE_FONT_SIZE * 9 + TITLE_FONT_SMALL_SIZE, titleFontSmall, licenceText3.GetString());
		app->fonts->DrawText(84, 84 + TITLE_FONT_SIZE * 10 + TITLE_FONT_SMALL_SIZE, titleFontSmall, licenceText4.GetString());
		app->fonts->DrawText(84, 84 + TITLE_FONT_SIZE * 12 + TITLE_FONT_SMALL_SIZE, titleFontSmall, licenceText5.GetString());
		app->fonts->DrawText(84, 84 + TITLE_FONT_SIZE * 13 + TITLE_FONT_SMALL_SIZE, titleFontSmall, licenceText6.GetString());
		btnBack->Draw();
	}

	//chckTest->Draw();
	//sldrTest->Draw();

	return ret;
}

bool TitleScene::CleanUp()
{
	LOG("Freeing scene");

	app->tex->UnLoad(titleScreen);

	app->gui->DestroyGuiControl(btnPlay);
	btnPlay = nullptr;
	app->gui->DestroyGuiControl(btnContinue);
	btnContinue = nullptr;
	app->gui->DestroyGuiControl(btnSettings);
	btnSettings = nullptr;
	app->gui->DestroyGuiControl(btnCredits);
	btnCredits = nullptr;
	app->gui->DestroyGuiControl(btnExit);
	btnExit = nullptr;
	app->gui->DestroyGuiControl(btnBack);
	btnBack = nullptr;

	return true;
}

bool TitleScene::OnGuiMouseClickEvent(GuiControl* control)
{
	switch (control->type)
	{
	case GuiControlType::BUTTON:
	{
		switch (control->id)
		{
		case 1:	//Play
		{
			app->transition->FadeEffect(this, (Module*)app->scene, false, floor(1200.0f * dtTmp));
			break;
		}
		case 4: //Credits
		{
			credits = true;
			break;
		}
		case 5:	//Exit
		{
			exitRequest = true;
			break;
		}
		case 6: //Back
		{
			credits = false;
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