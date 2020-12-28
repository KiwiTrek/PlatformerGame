#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"

#include "DynArray.h"
#include "Point.h"
#include "SString.h"

enum EnemyType;
class EntityManager;
class Collider;

class Enemy : public Entity
{
public:

    Enemy(int x, int y, EntityManager* listener, EnemyType eType);
    virtual ~Enemy();

    // Collision response
    void OnCollision(Collider* c1, Collider* c2) override;

public:
    DynArray<iPoint> path;

    // State changes
    bool attackChange = false;
    bool hurtChange = false;
};

#endif // __ENEMY_H__
