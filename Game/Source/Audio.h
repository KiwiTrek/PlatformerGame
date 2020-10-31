#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "Module.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

class Audio : public Module
{
public:

	Audio();

	// Destructor
	virtual ~Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fadeTime = DEFAULT_MUSIC_FADE_TIME);

	// Load/Unload a WAV in memory
	unsigned int LoadFx(const char* path);
	bool UnloadFx(unsigned int index);
	bool SetFxVolume(unsigned int index);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

	// Set volume to 0
	void MuteVolume();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&);

private:

	_Mix_Music* music;
	List<Mix_Chunk *> fx;

	int volumeFx;
};

#endif // __AUDIO_H__