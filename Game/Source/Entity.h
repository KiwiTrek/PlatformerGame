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

    Entity(int x, int y, EntityType type, EnemyType eType = EnemyType::NO_TYPE) : type(type), active(true) {}

    virtual bool Update(float dt)
    {
        return true;
    }

    // Collision response
    virtual void OnCollision(Collider* c1, Collider* c2)
    {}

    virtual bool IsIdle()
    {
        return false;
    }

public:

    EntityType type;
    bool active = true;
    //SString name;         // Entity name identifier?
    //uint32 id;            // Entity identifier?

    // Possible properties, it depends on how generic we
    // want our Entity class, maybe it's not renderable...
    iPoint position;        // Use a float instead?
    bool renderable = false;
    SDL_Texture* texture;
    SDL_Rect rect;
    Physics physics;
    iPoint nextPos;
    DynArray<iPoint> path;
    int pathCount;
    int destroyedFx;
    Collider* collider;

    // Original spawn position
    iPoint spawnPos;

    //Pathfinding related variables
    iPoint pastDest;
    int i;
    int counterTile;

    // Animation related variables
    Animation* currentAnim = nullptr;
    bool invert = false;
};

#endif // __ENTITY_H__