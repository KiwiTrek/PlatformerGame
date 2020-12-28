#ifndef __MAP_H__
#define __MAP_H__

#include "Module.h"

#include "DynArray.h"
#include "List.h"
#include "PQueue.h"
#include "Point.h"

#include "SDL/include/SDL.h"

class Textures;
class Render;

struct Properties
{
	struct Property
	{
		SString name;
		int value;
	};

	~Properties()
	{
		ListItem<Property*>* item;
		item = list.start;

		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}

		list.Clear();
	}

	int GetProperty(const char* name, int defaultValue = 0) const;
	void SetProperty(const char* name, int value);

	List<Property*> list;
};

struct Tile
{
	int id;
	Properties properties;
};

// L03: DONE 2: Create a struct to hold information for a TileSet
// Ignore Terrain Types and Tile Types for now, but we want the image!
struct TileSet
{
	SString	name;
	int	firstgid;
	int margin;
	int	spacing;
	int	tileWidth;
	int	tileHeight;

	SDL_Texture* texture;
	int	texWidth;
	int	texHeight;
	int	numTilesWidth;
	int	numTilesHeight;
	int	offsetX;
	int	offsetY;

	List<Tile*> tileSetPropList;

	// Receives a tile id and returns it's Rect position on the tileset
	SDL_Rect GetTileRect(int id) const;

	// Create a function that gives Id and returns properties
	Tile* GetPropList(int id) const;
};

// L03: DONE 1: We create an enum for map type, just for convenience,
// NOTE: Platformer game will be of type ORTHOGONAL
enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};

// L04: DONE 1: Create a struct for the map layer
struct MapLayer
{
	SString	name;
	int width;
	int height;
	uint* data;

	// L06: DONE 1: Support custom properties
	Properties properties;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	// L04: DONE 6: Short function to get the value of x,y
	inline uint Get(int x, int y) const
	{
		return data[(y * width) + x];
	}
};

// L03: DONE 1: Create a struct needed to hold the information to Map node
struct MapData
{
	int width;
	int	height;
	int	tileWidth;
	int	tileHeight;
	SDL_Color backgroundColor;
	MapTypes type;
	List<TileSet*> tileSets;

	// L04: DONE 2: Add a list/array of layers to the map
	List<MapLayer*> mapLayer;
};

class Map : public Module
{
public:
	// Constructor
    Map(Textures* textures);

    // Destructor
    virtual ~Map();

	// Called when program is executed
	void Init();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
    void Draw(Render* render);

	void DrawPath(Render* render, DynArray<iPoint>* path);

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	// Translates map position to world position
	iPoint MapToWorld(int x, int y) const;

	// Creates the data required to check the walkability of the whole map for pathfinding
	bool CreateWalkabilityMap(int* width, int* height, uchar** buffer) const;

	// Changes property to value assigned
	void SetTileProperty(int x, int y, const char* property, int value, bool notMovCollision = false, bool isObject = false);

	// Gets the value of a property in a given tile
	int GetTileProperty(int x, int y, const char* property, bool notMovCollision = false, bool isObject = false) const;

	// Stores the data of the whole map
	MapData data;

private:
	bool LoadMap();
	bool LoadTileSetDetails(pugi::xml_node& node, TileSet* set);
	bool LoadTileSetImage(pugi::xml_node& node, TileSet* set);
	bool LoadTileSetProperties(pugi::xml_node& node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);
	MapTypes StrToMapType(SString str);
	TileSet* GetTileSetFromTileId(int id) const;
	bool StoreId(pugi::xml_node& node, MapLayer* layer, int index);
	void LogInfo();

	pugi::xml_document mapFile;
	SString folder;
	bool mapLoaded;
	SString folderTexture;
	SDL_Texture* debugPath;

	Textures* tex;
};

#endif // __MAP_H__