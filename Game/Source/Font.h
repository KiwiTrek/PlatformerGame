#ifndef __FONT_H__
#define __FONT_H__

#include "Defs.h"

#include "SDL/include/SDL.h"

class Font
{
public:

    Font(SDL_Texture* atlas, int count, int rows, int cols, int padding);

    ~Font();

    void SetFirstCharacter(int value);

    SDL_Rect GetCharRec(int value);

private:
	
	SDL_Texture* texture;

    int baseSize;
    int charsCount;
    SDL_Rect charsRecs[128];

    int firstChar;
};

#endif //__FONT_H__