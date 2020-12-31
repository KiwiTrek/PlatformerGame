#include "GuiManager.h"

#include "App.h"
#include "Textures.h"
#include "Fonts.h"

#include "GuiButton.h"

GuiManager::GuiManager() : Module()
{
	name.Create("guiManager");
}

GuiManager::~GuiManager()
{}

void GuiManager::Init()
{
	active = true;
}

bool GuiManager::Awake(pugi::xml_node& config)
{
	bool ret = true;

	folderTexture.Create(config.child("folderTexture").child_value());

	return ret;
}

bool GuiManager::Start()
{
	SString tmp("%s%s", folderTexture.GetString(), "button_atlas.png");
	atlas = app->tex->Load(tmp.GetString());
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "default_label.png");
	defaultFont = app->fonts->Load(tmp.GetString(), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,_-@#*^()[]<>: ", 3);
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "disabled_label.png");
	disabledFont = app->fonts->Load(tmp.GetString(), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,_-@#*^()[]<>: ", 3);
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "hover_label.png");
	hoverFont = app->fonts->Load(tmp.GetString(), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,_-@#*^()[]<>: ", 3);
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "pressed_label.png");
	pressedFont = app->fonts->Load(tmp.GetString(), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,_-@#*^()[]<>: ", 3);
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "title_label.png");
	titleFont = app->fonts->Load(tmp.GetString(), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,_-@#*^()[]<>: ", 3);
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "title_label_small.png");
	titleFontSmall = app->fonts->Load(tmp.GetString(), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,_-@#*^()[]<>: ", 3);

	return true;
}

GuiControl* GuiManager::CreateGuiControl(GuiControlType type, uint32 id, SDL_Rect bounds, const char* text, Module* observer)
{
	GuiControl* control = nullptr;

	switch (type)
	{
	case GuiControlType::BUTTON:
	{
		control = new GuiButton(id, bounds, text);
		break;
	}
	case GuiControlType::CHECKBOX:
	{
		break;
	}
	case GuiControlType::SLIDER:
	{
		break;
	}
	default: break;
	}

	control->SetObserver(observer);
	control->SetTexture(atlas);
	control->SetFonts(defaultFont, titleFont, hoverFont, pressedFont, disabledFont);

	// Created entities are added to the list
	if (control != nullptr) controls.Add(control);

	return control;
}

bool GuiManager::CleanUp()
{
	app->tex->UnLoad(atlas);

	app->fonts->Unload(defaultFont);
	app->fonts->Unload(disabledFont);
	app->fonts->Unload(hoverFont);
	app->fonts->Unload(pressedFont);
	app->fonts->Unload(titleFont);

	return true;
}