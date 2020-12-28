#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "Module.h"

#include "Entity.h"

#include "List.h"
#include "Animation.h"
#include "DynArray.h"

#define MAX_ENTITIES 100

enum EnemyType;
class Entity;
class Collisions;
class Render;
class Textures;
class Map;
class Input;
class AudioManager;
class App;
enum class EntityType;

class EntityManager : public Module
{
public:

	EntityManager(Render* renderM, Textures* textures, Input* inputM, Map* mapM, AudioManager* audioM, App* appM);

	// Destructor
	virtual ~EntityManager();

	// Called when program is executed
	void Init();

	// Called before render is available
	bool Awake(pugi::xml_node&);

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

	Collisions* collisions;
	Render* render;
	Textures* tex;
	Map* map;
	AudioManager* audio;
	Input* input;
	App* app;

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
};

#endif // __ENTITYMANAGER_H__
