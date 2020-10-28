#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#define MAX_COLLIDERS 75

#include "Module.h"

#include "SDL/include/SDL.h"

class Collider {
public:
    enum TYPE {
        NONE = -1,
        AIR,
        SOLID,
        SPIKE,
        PLAYER,
        FRUIT,
        SPAWN,
        GOAL,
        MAX
    };

    Collider(SDL_Rect _rect, TYPE _type, Module* _listener = nullptr) : rect(_rect), type(_type), listener(_listener) {}

    void SetPos(int _x, int _y, int _w, int _h);

    bool Intersects(const SDL_Rect& r) const;

    SDL_Rect rect;
    bool pendingToDelete = false;
    TYPE type;
    Module* listener = nullptr;
};

class Collisions : public Module {
public:

	Collisions();

	void Init();

	// Destructor
	virtual ~Collisions();

	// Called before player is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&);

	// Adds a new collider to the list
	Collider* AddCollider(SDL_Rect rect, Collider::TYPE type, Module* listener = nullptr);

	// Draws all existing colliders with some transparency
	// void DebugDraw();		<- we dont need this do we?

private:
    // All existing colliders in the scene
    Collider* colliders[MAX_COLLIDERS] = { nullptr };

    // The collision matrix. Defines the interaction for two collider types
    // If set two false, collider 1 will ignore collider 2
    bool matrix[Collider::TYPE::MAX][Collider::TYPE::MAX];

    // Simple debugging flag to draw all colliders
    // bool debug = false;      <- same here i think
};

#endif // !__COLLISIONS_H__

