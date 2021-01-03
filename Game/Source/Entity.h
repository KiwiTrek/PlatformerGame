#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "Point.h"
#include "SString.h"
#include "Animation.h"
#include "Physics.h"
#include "DynArray.h"

class Collider;

enum class EntityType
{
    PLAYER,
    ENEMY,
    COIN,
    UNKNOWN
};

enum EnemyType
{
    NO_TYPE,
    GROUND,
    FLYING
};

class Entity
{
public:

    Entity(int x, int y, EntityType type, EnemyType eType = EnemyType::NO_TYPE) : type(type), eType(eType), active(true) {}

    virtual bool Update(float dt)
    {
        return true;
    }

    virtual bool PostUpdate(float dt)
    {
        return true;
    }

    virtual bool Draw()
    {
        return true;
    }

    // Collision response
    virtual void OnCollision(Collider* c1, Collider* c2)
    {}

public:

    EntityType type;
    bool active = true;
    //SString name;         // Entity name identifier?
    //uint32 id;            // Entity identifier?

    // Possible properties, it depends on how generic we
    // want our Entity class, maybe it's not renderable...
    iPoint position;        // Use a float instead?
    SDL_Texture* entityTex;
    SDL_Rect entityRect;
    Physics physics;
    iPoint nextPos;
    Collider* collider;
    uint destroyedFx;

    bool pendingToDelete = false;

    // Original spawn position
    iPoint spawnPos;

    // Animation related variables
    Animation* currentAnim = nullptr;
    bool invert = false;

    //Public vars from player (SHOULD BE TMP)
    bool isDead = false;

    //Public vars from enemy (SHOULD BE TMP)
    EnemyType eType;
};

#endif // __ENTITY_H__