#ifndef __RENDER_H__
#define __RENDER_H__

#include "Module.h"

#include "Point.h"
#include "Font.h"

#include "SDL/include/SDL.h"

class Window;

class Render : public Module
{
public:
	// Constructor
	Render(Window* window);

	// Destructor
	virtual ~Render();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// ViewPort functions
	void SetViewPort(const SDL_Rect& rect);
	void ResetViewPort();

	// Drawing
	bool DrawTexture(SDL_Texture* texture, int x, int y, bool fullscreen = false, const SDL_Rect* section = NULL, bool invert = false, float speed = 1.0f, double angle = 0, int pivotX = INT_MAX, int pivotY = INT_MAX) const;
	bool DrawRectangle(const SDL_Rect& rect, SDL_Color color, bool filled = true, bool useCamera = true) const;
	bool DrawLine(int x1, int y1, int x2, int y2, SDL_Color color, bool useCamera = true) const;
	bool DrawCircle(int x1, int y1, int radius, SDL_Color color) const;
	void SetBackgroundColor(SDL_Color color);

public:
	// Debug drawing
	bool drawAll;

	SDL_Renderer* renderer;
	SDL_Rect camera = { 0,0,0,0 };
	SDL_Rect viewport;
	SDL_Color background;
	uint scale;

	Window* win;
};

#endif // __RENDER_H__