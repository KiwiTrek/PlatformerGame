#include "SceneGameplay.h"

#include "EntityManager.h"
#include "Audio.h"
#include "Render.h"

SceneGameplay::SceneGameplay()
{
}

SceneGameplay::~SceneGameplay()
{}

bool SceneGameplay::Load(Textures* tex, EntityManager* entityManager)
{
	// L03: DONE: Load map
	// L12b: Create walkability map on map loading
	if (map->Load("platformer.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;

		//if (map->CreateWalkabilityMap(w, h, &data)) pathFinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	// Load music
	//AudioManager::PlayMusic("Assets/Audio/Music/music_spy.ogg");

	// Load game entities
	//Player* player = (Player*)entityManager->CreateEntity(EntityType::PLAYER);
	//player->SetTexture(tex->Load("Assets/Textures/player.png"));
	//entityManager->CreateEntity(EntityType::ENEMY);
	//entityManager->CreateEntity(EntityType::ENEMY);
	//entityManager->CreateEntity(EntityType::ITEM);
	//entityManager->CreateEntity(EntityType::ITEM);
	//entityManager->CreateEntity(EntityType::ITEM);

	// Initialize player
	player = (Player*)entityManager->CreateEntity(20, 20, EntityType::PLAYER);
	player->position = iPoint(200, 400);

    return false;
}

inline bool CheckCollision(SDL_Rect rec1, SDL_Rect rec2)
{
	if ((rec1.x < (rec2.x + rec2.w) && (rec1.x + rec1.w) > rec2.x) &&
		(rec1.y < (rec2.y + rec2.h) && (rec1.y + rec1.h) > rec2.y)) return true;
	else return false;
}

bool SceneGameplay::Update(Input *input, Render* render, AudioManager* audio, float dt)
{
	// Collision detection: map vs player
	iPoint tempPlayerPosition = player->position;

	player->Update(input, map, render, audio, dt);

	// Check if updated player position collides with next tile
	// IMPROVEMENT: Just check adyacent tiles to player
	//for (int y = 0; y < map->data.height; y++)
	//{
	//	for (int x = 0; x < map->data.width; x++)
	//	{
	//		if ((map->data.mapLayer[2]->Get(x, y) >= 484) && CheckCollision(map->GetTilemapRec(x, y), player->GetBounds()))
	//		{
	//			player->position = tempPlayerPosition;
	//			player->jumpSpeed = 0.0f;
	//			break;
	//		}
	//	}
	//}

	// L02: DONE 3: Request Load / Save when pressing L/S
	//if (input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) app->LoadGameRequest();
	//if (input->GetKey(SDL_SCANCODE_S) == KEY_DOWN) app->SaveGameRequest();

	return true;
}

bool SceneGameplay::Draw(Render* render)
{
	// Draw map
	map->Draw(render);

	player->Draw(render);

    return false;
}

bool SceneGameplay::Unload()
{
	// TODO: Unload all resources

    return false;
}
