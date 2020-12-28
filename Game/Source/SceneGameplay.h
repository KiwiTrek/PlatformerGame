#ifndef __SCENEGAMEPLAY_H__
#define __SCENEGAMEPLAY_H__

#include "Scene.h"

#include "Map.h"
#include "Player.h"

class SceneGameplay : public Scene
{
public:

    SceneGameplay();
    virtual ~SceneGameplay();

    bool Load(Textures* tex, EntityManager* entityManager);

    bool Update(Input* input, Render* render, AudioManager* audio, float dt);

    bool Draw(Render* render);

    bool Unload();

private:

    Map* map;
    Player* player;

    SDL_Rect camera;
};

#endif // __SCENEGAMEPLAY_H__
