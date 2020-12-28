#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#include "SDL.h"

#define MAX_COLLIDERS 75

class Module;

class Collider
{
public:
	// Collision Type enum
	enum Type
	{
		NONE = -1,
		AIR,
		SOLID,
		SPIKE,
		PLAYER,
		FRUIT,
		CHECKPOINT,
		GOAL,
		ENEMY,
		ATTACK,
		MAX
	};

	// Constructor
	Collider(SDL_Rect rect, Type type, Module* listener = nullptr) : rect(rect), type(type), listener(listener)
	{}

	// Sets the position of the construct
	void SetPos(int x, int y, int w, int h);

	// Checks if two rects are intersecting
	bool Intersects(const SDL_Rect& r) const;

public:
	SDL_Rect rect;
	bool pendingToDelete = false;
	Type type;
	Module* listener = nullptr;
};

class Collisions
{
public:
	// Constructor
	Collisions();

	// Called each loop iteration
	bool PreUpdate();

	// Called before quitting
	bool CleanUp();

	// Adds a new collider to the list
	Collider* AddCollider(SDL_Rect rect, Collider::Type type, Module* listener = nullptr);

private:
	// All existing colliders in the scene
	Collider* colliders[MAX_COLLIDERS] = { nullptr };

	// The collision matrix. Defines the interaction for two collider types
	// If set to false, collider 1 will ignore collider 2
	bool matrix[Collider::Type::MAX][Collider::Type::MAX];
};

#endif // __COLLISIONS_H__