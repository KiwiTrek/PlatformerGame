#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "SDL/include/SDL_rect.h"

#define MAX_FRAMES 1000

class Animation
{
public:
	void PushBack(const SDL_Rect& rect)
	{
		frames[totalFrames++] = rect;
	}
	void Reset()
	{
		currentFrame = 0;
		loopCount = 0;
	}
	bool HasFinished()
	{
		return !loop && loopCount > 0;
	}
	void Update()
	{
		currentFrame += speed;
		if (currentFrame >= totalFrames)
		{
			if (loop)
			{
				currentFrame = 0.0f;
			}
			else
			{
				currentFrame = totalFrames - 1;
			}
			++loopCount;
		}
	}

	SDL_Rect& GetCurrentFrame()
	{
		return frames[(int)currentFrame];
	}

	void SetLoop(bool _loop)
	{
		loop = _loop;
	}

	float GetSpeed() const
	{
		return speed;
	}

	void SetSpeed(float _speed)
	{
		speed = _speed;
	}

	int GetHeight()
	{
		return frames[(int)currentFrame].h;
	}

	int GetWidth()
	{
		return frames[(int)currentFrame].w;
	}

private:
	float currentFrame = 0.0f;
	int totalFrames = 0;
	int loopCount = 0;
	bool loop = true;
	float speed = 1.0f;
	SDL_Rect frames[MAX_FRAMES];
};

#endif //__ANIMATION_H__