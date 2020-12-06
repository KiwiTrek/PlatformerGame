#ifndef __MODULETRANSITION_H__
#define __MODULETRANSITION_H__

#include "Module.h"
#include "SDL\include\SDL_rect.h"

class Transition : public Module
{
public:
	// Constructor
	Transition();

	//Destructor
	~Transition();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate();

	// Called from another module
	// Starts the fade process which has two steps, fade_out and fade_in
	// After the first step, the modules should be switched
	bool FadeEffect(Module* toDisable, Module* toEnable, bool fadeInOnly, float frames = 60.0f);

private:
	enum TransitionStep
	{
		NONE,
		TO_BLACK,
		FROM_BLACK
	}

	currentStep = TransitionStep::NONE;

	// A frame count system to handle the fade time and ratio
	Uint32 frameCount = 0;
	Uint32 maxFadeFrames = 0;
	float fadeRatio = 0.0f;

	// The rectangle of the screen, used to render the black rectangle
	SDL_Rect screenRect;

	// The storing of the modules to switch
	Module* moduleToEnable = nullptr;
	Module* moduleToDisable = nullptr;
};

#endif //__MODULEFADETOBLACK_H__