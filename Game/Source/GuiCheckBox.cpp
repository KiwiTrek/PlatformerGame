#include "GuiCheckBox.h"

#include "App.h"
#include "Input.h"
#include "Render.h"

GuiCheckBox::GuiCheckBox(uint32 id, SDL_Rect bounds, const char* text) : GuiControl(GuiControlType::CHECKBOX, id)
{
    this->bounds = bounds;
    this->text = text;
}

GuiCheckBox::~GuiCheckBox()
{
}

bool GuiCheckBox::Update(float dt)
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

            // If mouse button pressed -> Generate event!
            if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP)
            {
                checked = !checked;
                NotifyObserver();
            }
        }
        else state = GuiControlState::NORMAL;
    }

    return false;
}

bool GuiCheckBox::Draw()
{
    // Draw the right button depending on state
    switch (state)
    {
    case GuiControlState::DISABLED:
    {
        if (checked)
        {

        }
        else
        {

        }
        break;
    }
    case GuiControlState::NORMAL:
    {
        if (checked)
        {

        }
        else
        {

        }
        break;
    }
    case GuiControlState::FOCUSED:
    {
        if (checked)
        {

        }
        else
        {

        }
        break;
    }
    case GuiControlState::PRESSED:
    {
        if (checked)
        {

        }
        else
        {

        }
        break;
    }
    case GuiControlState::SELECTED:
    {
        if (checked)
        {

        }
        else
        {

        }
        break;
    }
    default:
    {
        break;
    }
    }

    return false;
}
