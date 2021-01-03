#include "GuiManager.h"

#include "App.h"
#include "Textures.h"
#include "Fonts.h"

#include "GuiButton.h"
#include "GuiCheckBox.h"
#include "GuiSlider.h"

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
	tmp.Create("%s%s", folderTexture.GetString(), "default_label_small.png");
	defaultFontSmall = app->fonts->Load(tmp.GetString(), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,_-@#*^()[]<>: ", 3);
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "disabled_label.png");
	disabledFont = app->fonts->Load(tmp.GetString(), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,_-@#*^()[]<>: ", 3);
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "disabled_label_small.png");
	disabledFontSmall = app->fonts->Load(tmp.GetString(), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,_-@#*^()[]<>: ", 3);
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "hover_label.png");
	hoverFont = app->fonts->Load(tmp.GetString(), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,_-@#*^()[]<>: ", 3);
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "hover_label_small.png");
	hoverFontSmall = app->fonts->Load(tmp.GetString(), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,_-@#*^()[]<>: ", 3);
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "pressed_label.png");
	pressedFont = app->fonts->Load(tmp.GetString(), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,_-@#*^()[]<>: ", 3);
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "pressed_label_small.png");
	pressedFontSmall = app->fonts->Load(tmp.GetString(), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,_-@#*^()[]<>: ", 3);
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "title_label.png");
	titleFont = app->fonts->Load(tmp.GetString(), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,_-@#*^()[]<>: ", 3);
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "title_label_medium.png");
	titleFontMedium = app->fonts->Load(tmp.GetString(), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,_-@#*^()[]<>: ", 3);
	tmp.Clear();
	tmp.Create("%s%s", folderTexture.GetString(), "title_label_small.png");
	titleFontSmall = app->fonts->Load(tmp.GetString(), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,_-@#*^()[]<>: ", 3);

	return true;
}

GuiControl* GuiManager::CreateGuiControl(GuiControlType type, uint32 id, SDL_Rect bounds, const char* text, Module* observer, int widthInUnits, bool secondText, const char* text2)
{
	GuiControl* control = nullptr;

	switch (type)
	{
	case GuiControlType::BUTTON:
	{
		if (secondText)
		{
			control = new GuiButton(id, bounds, text, text2);
		}
		else
		{
			control = new GuiButton(id, bounds, text);
		}
		break;
	}
	case GuiControlType::CHECKBOX:
	{
		control = new GuiCheckBox(id, bounds, text);
		break;
	}
	case GuiControlType::SLIDER:
	{
		control = new GuiSlider(id, bounds, widthInUnits, text);
		break;
	}
	default: break;
	}

	control->SetObserver(observer);
	control->SetTexture(atlas);
	if (secondText)
	{
		control->SetFonts(defaultFontSmall, titleFontMedium, hoverFontSmall, pressedFontSmall, disabledFontSmall);
	}
	else
	{
		control->SetFonts(defaultFont, titleFontMedium, hoverFont, pressedFont, disabledFont);
	}

	// Created entities are added to the list
	if (control != nullptr) controls.Add(control);

	return control;
}

void GuiManager::DestroyGuiControl(GuiControl* entity)
{
	delete entity;
	entity = nullptr;
}

bool GuiManager::CleanUp()
{
	app->tex->UnLoad(atlas);

	app->fonts->Unload(defaultFont);
	app->fonts->Unload(disabledFont);
	app->fonts->Unload(hoverFont);
	app->fonts->Unload(pressedFont);
	app->fonts->Unload(titleFont);
	app->fonts->Unload(defaultFontSmall);
	app->fonts->Unload(disabledFontSmall);
	app->fonts->Unload(hoverFontSmall);
	app->fonts->Unload(pressedFontSmall);
	app->fonts->Unload(titleFontMedium);
	app->fonts->Unload(titleFontSmall);

	return true;
}