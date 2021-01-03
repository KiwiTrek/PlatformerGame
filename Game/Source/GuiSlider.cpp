#include "GuiSlider.h"

#include "App.h"
#include "Input.h"
#include "Render.h"
#include "Fonts.h"

#include "Log.h"

GuiSlider::GuiSlider(uint32 id, SDL_Rect bounds, int widthInUnits, const char* text) : GuiControl(GuiControlType::SLIDER, id)
{
	this->bounds = bounds;
	this->widthInUnits = widthInUnits;
	int width = widthInUnits * 54;
	this->limits = { bounds.x, bounds.y + (bounds.h / 2), width, 5 };
	this->bounds.y = bounds.y + (bounds.h / 4);
	this->text = text;
	this->state = GuiControlState::NORMAL;
	this->offsetText = this->text.Length() * 24 + (24 * 3);

	normal = { 271,0,54,54 };
	focused = { 271,109,54,54 };
	pressed = { 271,218,54,54 };
	disabled = { 271,327,54,54 };

	normalLimitsBegin = { 271,54,54,54 };
	normalLimitsMiddle = { 325,54,54,54 };
	normalLimitsEnd = { 379,54,54,54 };

	disabledLimitsBegin = { 271,381,54,54 };
	disabledLimitsMiddle = { 325,381,54,54 };
	disabledLimitsEnd = { 379,381,54,54 };
}

GuiSlider::~GuiSlider()
{
}

bool GuiSlider::Update(float dt)
{
	int tmpValue = (float)maxValue / (float)(limits.w - bounds.w);
	value = (bounds.x - limits.x) * tmpValue;
	if (state != GuiControlState::DISABLED)
	{
		int mouseX, mouseY;
		app->input->GetMousePosition(mouseX, mouseY);

		int motionX = 0, motionY = 0;
		app->input->GetMouseMotion(motionX, motionY);

		// Check collision between mouse and button bounds
		if ((mouseX > bounds.x) && (mouseX < (bounds.x + bounds.w)) &&
			(mouseY > bounds.y) && (mouseY < (bounds.y + bounds.h)))
		{
			state = GuiControlState::FOCUSED;

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT)
			{
				state = GuiControlState::PRESSED;
			}
		}
		else if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) != KeyState::KEY_REPEAT)
		{
			state = GuiControlState::NORMAL;
		}

		if (motionX != 0 && state == GuiControlState::PRESSED)
		{
			bounds.x = mouseX - (bounds.w/2);
			NotifyObserver();
		}

		if (bounds.x < limits.x)
		{
			bounds.x = limits.x;
		}
		if ((bounds.x + bounds.w) >= (limits.x + limits.w))
		{
			bounds.x = limits.x + limits.w - bounds.w;
		}

		if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
		{
			state = GuiControlState::NORMAL;
		}
	}

	return false;
}

bool GuiSlider::Draw()
{
	app->fonts->DrawText(limits.x - offsetText, limits.y, titleFont, text.GetString());

	for (int i = 1; i != widthInUnits - 1; ++i)
	{
		app->render->DrawTexture(texture, limits.x + (i * 54), limits.y, false, &normalLimitsMiddle);
	}
	app->render->DrawTexture(texture, limits.x, limits.y, false, &normalLimitsBegin);
	app->render->DrawTexture(texture, limits.x + limits.w - bounds.w, limits.y, false, &normalLimitsEnd);

	// Draw the right button depending on state
	switch (state)
	{
	case GuiControlState::DISABLED:
	{
		for (int i = 1; i != widthInUnits - 1; ++i)
		{
			app->render->DrawTexture(texture, limits.x + (i * 54), limits.y, false, &disabledLimitsMiddle);
		}
		app->render->DrawTexture(texture, limits.x, limits.y, false, &disabledLimitsBegin);
		app->render->DrawTexture(texture, limits.x + limits.w - bounds.w, limits.y, false, &disabledLimitsEnd);

		app->render->DrawTexture(texture, bounds.x, bounds.y + (bounds.h / 4), false, &disabled);
		break;
	}
	case GuiControlState::NORMAL:
	{
		app->render->DrawTexture(texture, bounds.x, bounds.y + (bounds.h / 4), false, &normal);
		break;
	}
	case GuiControlState::FOCUSED:
	{
		app->render->DrawTexture(texture, bounds.x, bounds.y + (bounds.h / 4), false, &focused);
		break;
	}
	case GuiControlState::PRESSED:
	{
		app->render->DrawTexture(texture, bounds.x, bounds.y + (bounds.h / 4), false, &pressed);
		break;
	}
	default:
	{
		break;
	}
	}

	return false;
}
