#ifndef __MODULE_ENTITY_MANAGER_H__
#define __MODULE_ENTITY_MANAGER_H__

#include "Module.h"
#include "Entity.h"
#include "List.h"
#include "GuiButton.h"
#include "GuiCheckBox.h"
#include "GuiSlider.h"

#define MAX_ENTITIES 100

class Entity;

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

	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Additional methods
	Entity* CreateEntity(int x, int y, EntityType type, Entity* playerPointer = nullptr, EnemyType eType = EnemyType::NO_TYPE, int listNumber = 0);
	void DestroyEntity(Entity* entity);

	bool UpdateAll(float dt, bool doLogic);

	// Collision response
	void OnCollision(Collider* c1, Collider* c2);

	// Declare on mouse click event
	bool OnGuiMouseClickEvent(GuiControl* control);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&);

public:

	List<Entity*> entities;
	List<Entity*> loadingEntities;

	float accumulatedTime = 0.0f;
	float updateMsCycle = 0.0f;
	bool doLogic = false;

	SString folderTexture;
	SString folderAudioFx;
	SString folderMap;

	// The audio fx for destroying an enemy
	int enemyDestroyedFx = 0;
	uint deadFx;
	uint jumpFx;
	uint doubleJumpFx;
	uint fruitFx;
	uint hitFx;
	uint slashFx;
	uint checkpointFx;
	uint coinFx;

	SDL_Texture* playerTex;
	SDL_Texture* playerHeart;
	SDL_Texture* ground;
	SDL_Texture* flying;
	SDL_Texture* coin;

	// Pause menu
	iPoint cameraPos;
	iPoint cameraSize;

	GuiButton* btnSettings;
	SString settingsTitle;
	bool settings;
	int offsetSettings;
	GuiSlider* sldrMusic;
	GuiSlider* sldrFx;
	GuiCheckBox* chckFullscreen;
	GuiCheckBox* chckVsync;
	GuiButton* btnBack;

	int pauseFont;
	SString pauseTitle;
	int offsetTitle;
	GuiButton* btnResume;
	GuiButton* btnTitle;
	float dtTmp;
	GuiButton* btnExit;
	bool exitRequest;
};

#endif // __MODULE_ENTITY_MANAGER_H__