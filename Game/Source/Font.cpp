#include "Font.h"

Font::Font(SDL_Texture* atlas, int count, int rows, int cols, int padding)
{
    // NOTE: To load a bitmap font atlas we must adjust to some constraints:
    // - Characters are ordered: Start at first char value and consecutive values follow
    // - All characters width is the same
    // - All characters have the same padding surrounding them
    // - A maximum of 128 characters are supported

    texture = atlas;
    charsCount = count;
    firstChar = 32;         // Default to space
    //baseSize = ((atlas->width / rows) - 2 * padding);

    // TODO: Define the multiple SDL_Recs, one for every character
    for (int i = 0; i < 128; i++)
    {
        //charsRecs[i] = SDL_Rect()
    }
}

Font::~Font()
{
}

void Font::SetFirstCharacter(int value)
{
    firstChar = value;
}

SDL_Rect Font::GetCharRec(int value)
{
    SDL_Rect rec = { 0 };

    // TODO: Get character rectangle corresponding to introduced value

    return rec;
}
