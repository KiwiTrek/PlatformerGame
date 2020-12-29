#ifndef __MODULE_ENEMIES_H__
#define __MODULE_ENEMIES_H__

#include "Module.h"
#include "Enemy.h"

#define MAX_ENEMIES 100

// Needed to spawn enemies
struct EnemySpawnpoint
{
	EnemyType type = EnemyType::NO_TYPE;
	int x, y;
};

class Enemy;
enum EnemyType;
struct SDL_Texture;

class EnemyManagement : public Module
{
public:
	// Constructor
	EnemyManagement();

	// Destructor
	~EnemyManagement();

	// Called when program is executed
	void Init();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	// Loads the necessary textures for the enemies
	bool Start() override;

	// Called each loop iteration
	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	// Destroys all active enemies left in the array
	bool CleanUp() override;

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&);

	// Called when an enemi collider hits another collider
	// The enemy is destroyed and an explosion particle is fired
	void OnCollision(Collider* c1, Collider* c2);

	// Add an enemy into the queue to be spawned later
	bool AddEnemy(EnemyType type, int x, int y, uint i = -1);

	// Iterates the queue and checks for camera position
	void HandleEnemiesSpawn();

	// Destroys any enemies that have moved outside the camera limits
	void HandleEnemiesDespawn();

private:
	// Spawns a new enemy using the data from the queue
	void SpawnEnemy(const EnemySpawnpoint& info);

private:
	// A queue with all spawn points information
	EnemySpawnpoint spawnQueue[MAX_ENEMIES];

	// All spawned enemies in the scene
	Enemy* enemies[MAX_ENEMIES] = { nullptr };

	// The enemies sprite sheet
	SDL_Texture* ground = nullptr;
	SDL_Texture* flying = nullptr;

	SString folderTexture;
	SString folderAudioFx;

	// The audio fx for destroying an enemy
	int enemyDestroyedFx = 0;
};

#endif // __MODULE_ENEMIES_H__