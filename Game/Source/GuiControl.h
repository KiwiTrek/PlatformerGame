#ifndef __GUICONTROL_H__
#define __GUICONTROL_H__

#include "Module.h"
#include "Point.h"
#include "SString.h"

#include "SDL/include/SDL.h"

enum class GuiControlType
{
	BUTTON,
	CHECKBOX,
	SLIDER,
};

enum class GuiControlState
{
	DISABLED,
	NORMAL,
	FOCUSED,
	PRESSED,
};

class GuiControl
{
public:

	GuiControl(GuiControlType type, uint32 id) : type(type), id(id), state(GuiControlState::NORMAL) {}

	GuiControl(GuiControlType type, SDL_Rect bounds, const char* text) :
		type(type),
		state(GuiControlState::NORMAL),
		bounds(bounds),
		text(text)
	{
		texture = NULL;
		defaultFont = -1;
		titleFont = -1;
		hoverFont = -1;
		pressedFont = -1;
		disabledFont = -1;
	}

	virtual bool Update(float dt)
	{
		return true;
	}

	virtual bool Draw() const
	{
		return true;
	}

	void SetTexture(SDL_Texture* tex)
	{
		texture = tex;
	}

	void SetFonts(int defaultId, int titleId, int hoverId, int pressedId, int disabledId)
	{
		defaultFont = defaultId;
		titleFont = titleId;
		hoverFont = hoverId;
		pressedFont = pressedId;
		disabledFont = disabledId;
	}

	void SetObserver(Module* module)
	{
		observer = module;
	}

	void NotifyObserver()
	{
		observer->OnGuiMouseClickEvent(this);
	}

public:

	uint32 id;
	GuiControlType type;
	GuiControlState state;

	SString text;           // Control text
	int offsetText;
	SDL_Rect bounds;        // Position and size

	SDL_Texture* texture;   // Texture atlas reference

	// Fonts
	int defaultFont;
	int titleFont;
	int hoverFont;
	int pressedFont;
	int disabledFont;

	Module* observer;        // Observer module
};

#endif // __GUICONTROL_H__