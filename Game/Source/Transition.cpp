#include "App.h"
#include "Window.h"
#include "Render.h"
#include "Transition.h"

#include "Defs.h"
#include "Log.h"

Transition::Transition() : Module()
{
	name.Create("transition");
}

Transition::~Transition()
{}

bool Transition::Start()
{
	LOG("Preparing Fade Screen");

	screenRect = { 0, 0, app->win->screenSurface->w * (int)app->win->GetScale(), app->win->screenSurface->h * (int)app->win->GetScale() };
	// Enable blending mode for transparency
	SDL_SetRenderDrawBlendMode(app->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

bool Transition::PreUpdate()
{
	return true;
}

bool Transition::Update(float dt)
{
	// Exit this function if we are not performing a fade
	if (currentStep == Transition_Step::NONE)
	{
		return true;
	}

	if (currentStep == Transition_Step::TO_BLACK)
	{
		++frameCount;
		if (frameCount >= maxFadeFrames)
		{
			moduleToDisable->Disable();
			moduleToEnable->Enable();
			currentStep = Transition_Step::FROM_BLACK;
		}
	}
	else
	{
		--frameCount;
		if (frameCount <= 0)
		{
			currentStep = Transition_Step::NONE;
		}
	}
	return true;
}

bool Transition::PostUpdate()
{
	// Exit this function if we are not performing a fade
	if (currentStep == Transition_Step::NONE)
	{
		return true;
	}

	float fadeRatio = (float)frameCount / (float)maxFadeFrames;

	// Render the black square with alpha on the screen
	SDL_SetRenderDrawColor(app->render->renderer, 0, 0, 0, (Uint8)(fadeRatio * 255.0f));
	SDL_RenderFillRect(app->render->renderer, &screenRect);

	return true;
}

bool Transition::FadeEffect(Module* moduleToDisable, Module* moduleToEnable, bool fadeInOnly, float frames)
{
	// If we are already in a fade process, ignore this call
	if (currentStep == Transition_Step::NONE)
	{
		if (fadeInOnly == false)
		{
			currentStep = Transition_Step::TO_BLACK;
			frameCount = 0;
			maxFadeFrames = frames;

			this->moduleToDisable = moduleToDisable;
			this->moduleToEnable = moduleToEnable;

			return true;
		}
		else
		{
			currentStep = Transition_Step::FROM_BLACK;
			frameCount = frames;
			maxFadeFrames = frames;

			return true;
		}
	}

	return false;
}