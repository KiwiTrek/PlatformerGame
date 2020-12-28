#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"

#include "Point.h"
#include "SString.h"

#include "SDL/include/SDL.h"

class EntityManager;
class Input;
class Map;
class Render;
class AudioManager;

class Player: public Entity
{
public:

    Player(int x, int y, EntityManager* listener);

    bool Update(Input* input, Map* map, Render* render, AudioManager* audio, float dt);

    bool Draw(Render* render);

    void SetTexture(SDL_Texture *tex);

    SDL_Rect GetBounds();

    // Collision response
    void OnCollision(Collider* c1, Collider* c2) override;

    // Gets the coordinates of the spawn point
    iPoint GetSpawnPoint();

    bool IsIdle();

public:
    //int width, height;

    //float jumpSpeed = 0.0f;
    //bool readyToJump = true;
    //bool hitObstacle = false;

    SDL_Texture* playerHeart;
    Collider* hurtBox = nullptr;

    int jumpCounter;
    int hitCD = 5;
    int lives = 3;
    int score = 0;

    bool keyPressed = false;
    bool godMode = false;
    bool isDead = false;
    bool isJumping = false;
    bool isHit = false;
    bool isAttacking = false;
    bool invert = false;
    bool debugDraw = false;
    bool once = true;
    bool onceCheckpoint = true;

    Animation idle;
    Animation run;
    Animation jumpPrep;
    Animation jumpMid;
    Animation jumpLand;
    Animation attack;
    Animation hit;
    Animation death;
    Animation wallJump;

    EntityManager* listener;
};

#endif // __PLAYER_H__
