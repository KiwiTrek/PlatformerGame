#ifndef __GUIBUTTON_H__
#define __GUIBUTTON_H__

#include "GuiControl.h"

#include "Point.h"
#include "SString.h"

class GuiButton : public GuiControl
{
public:

    GuiButton(uint32 id, SDL_Rect bounds, const char* text);
    GuiButton(uint32 id, SDL_Rect bounds, const char* text, const char* secondText);
    virtual ~GuiButton();

    bool Update(float dt);
    bool Draw();

private:
    SString secondText;
    int offsetTextSecond;
    bool secondaryText;
    // Gui Button specific properties
    SDL_Rect disabled;
    SDL_Rect normal;
    SDL_Rect focused;
    SDL_Rect pressed;
};

#endif // __GUIBUTTON_H__
