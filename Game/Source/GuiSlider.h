#ifndef __GUISLIDER_H__
#define __GUISLIDER_H__

#include "GuiControl.h"

#include "Point.h"
#include "SString.h"

class GuiSlider : public GuiControl
{
public:

    GuiSlider(uint32 id, SDL_Rect bounds, int widthInUnits, const char* text);
    virtual ~GuiSlider();

    bool Update(float dt);
    bool Draw(int cPosX = 0, int cPosY = 0);

public:
    SDL_Rect limits;

    int value;
    int maxValue;
    float conversion;

private:

    SDL_Rect normal;
    SDL_Rect focused;
    SDL_Rect pressed;
    SDL_Rect disabled;

    int widthInUnits;

    SDL_Rect normalLimitsBegin;
    SDL_Rect normalLimitsMiddle;
    SDL_Rect normalLimitsEnd;

    SDL_Rect disabledLimitsBegin;
    SDL_Rect disabledLimitsMiddle;
    SDL_Rect disabledLimitsEnd;
};

#endif // __GUISLIDER_H__
