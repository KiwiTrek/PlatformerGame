#ifndef __GUICHECKBOX_H__
#define __GUICHECKBOX_H__

#include "GuiControl.h"

#include "Point.h"
#include "SString.h"

class GuiCheckBox : public GuiControl
{
public:

    GuiCheckBox(uint32 id, SDL_Rect bounds, const char* text);
    virtual ~GuiCheckBox();

    bool Update(float dt);
    bool Draw();

private:

    bool checked;
    SDL_Rect disabled;
    SDL_Rect normal;
    SDL_Rect focused;
    SDL_Rect pressed;

    SDL_Rect disabledChecked;
    SDL_Rect normalChecked;
    SDL_Rect focusedChecked;
    SDL_Rect pressedChecked;
};

#endif // __GUICHECKBOX_H__
