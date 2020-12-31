#include "GuiSlider.h"

#include "App.h"
#include "Input.h"
#include "Render.h"

GuiSlider::GuiSlider(uint32 id, SDL_Rect bounds, int width, const char* text) : GuiControl(GuiControlType::SLIDER, id)
{
    this->bounds = bounds;
    this->limits = { bounds.x - width / 2, bounds.y + (bounds.h / 2), width, 5 };
    this->text = text;
    this->state = GuiControlState::NORMAL;
}

GuiSlider::~GuiSlider()
{
}

bool GuiSlider::Update(float dt)
{
    if (state != GuiControlState::DISABLED)
    {
        int mouseX, mouseY;
        app->input->GetMousePosition(mouseX, mouseY);

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

        int motionX, motionY;
        app->input->GetMouseMotion(motionX, motionY);
        if (motionX != 0 && state == GuiControlState::PRESSED)
        {
            bounds.x += motionX;
            if (bounds.x < limits.x /*|| (bounds.x + bounds.w) >(limits.x + limits.w)*/)
            {
                bounds.x = limits.x;
                // bounds.x -= motionX;
            }
            if ((bounds.x + bounds.w) > (limits.x + limits.w))
            {
                bounds.x = limits.x + limits.w - bounds.w;
            }
            NotifyObserver();
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
    app->render->DrawRectangle(limits, 0, 0, 0, 255);

    // Draw the right button depending on state
    switch (state)
    {
    case GuiControlState::DISABLED: app->render->DrawRectangle(bounds, 100, 100, 100, 255 );
        break;
    case GuiControlState::NORMAL: app->render->DrawRectangle(bounds, 0, 255, 0, 255 );
        break;
    case GuiControlState::FOCUSED: app->render->DrawRectangle(bounds, 255, 255, 0, 255 );
        break;
    case GuiControlState::PRESSED: app->render->DrawRectangle(bounds, 0, 255, 255, 255 );
        break;
    case GuiControlState::SELECTED: app->render->DrawRectangle(bounds, 0, 255, 0, 255 );
        break;
    default:
        break;
    }

    return false;
}
