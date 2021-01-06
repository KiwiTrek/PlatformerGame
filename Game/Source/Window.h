#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Module.h"

struct SDL_Window;
struct SDL_Surface;

class Window : public Module
{
public:
	// Constructor
	Window();

	// Destructor
	virtual ~Window();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Change/Set window title
	void SetTitle(const char* title);

	// Changes window state to/from fullscreen
	void ToggleFullscreen(bool fullscreen);

	// Returns the window size
	void GetWindowSize(uint& width, uint& height) const;

	// Returns the window scale
	uint GetScale() const;

public:
	// The window where rendering happens
	SDL_Window* window;

	// Determines window fullscreen
	bool fullscreenWindow;

	// The surface contained by the window
	SDL_Surface* screenSurface;

private:
	SString title;
	uint width;
	uint height;
	uint scale;
};

#endif // __WINDOW_H__