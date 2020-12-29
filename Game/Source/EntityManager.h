#ifndef __MODULE_ENTITY_MANAGER_H__
#define __MODULE_ENTITY_MANAGER_H__

#include "Module.h"
#include "Entity.h"
#include "List.h"

#define MAX_ENTITIES 100


class Entity;
enum EntityType;
enum EnemyType;

class EntityManager : public Module
{
public:

	EntityManager();

	// Destructor
	virtual ~EntityManager();

	// Called when program is executed
	void Init();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	// Loads the necessary textures for the entities
	bool Start();

	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Additional methods
	Entity* CreateEntity(int x, int y, EntityType type, EnemyType eType = EnemyType::NO_TYPE);
	void DestroyEntity(Entity* entity);

	bool UpdateAll(float dt, bool doLogic);

	// Collision response
	void OnCollision(Collider* c1, Collider* c2);

public:

	List<Entity*> entities;

	float accumulatedTime = 0.0f;
	float updateMsCycle = 0.0f;
	bool doLogic = false;

	SString folderTexture;
	SString folderAudioFx;

	// The audio fx for destroying an enemy
	int enemyDestroyedFx = 0;
	uint deadFx;
	uint jumpFx;
	uint doubleJumpFx;
	uint fruitFx;
	uint hitFx;
	uint slashFx;
	uint checkpointFx;

	SDL_Texture* playerTex;
	SDL_Texture* playerHeart;
	SDL_Texture* ground;
	SDL_Texture* flying;
};

#endif // __MODULE_ENTITY_MANAGER_H__