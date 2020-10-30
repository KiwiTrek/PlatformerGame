#ifndef __MAP_H__
#define __MAP_H__

#include "Module.h"
#include "List.h"
#include "Point.h"

#include "PugiXml\src\pugixml.hpp"

enum MapTypes
{
    MAPTYPE_UNKNOWN = 0,
    MAPTYPE_ORTHOGONAL,
    MAPTYPE_ISOMETRIC,
    MAPTYPE_STAGGERED
};

struct Properties
{
    struct Property
    {
        //...
        SString name;
        int value;
    };

    ~Properties()
    {
        //...
        ListItem<Property*>* item;
        item = list.start;

        while (item != NULL)
        {
            RELEASE(item->data);
            item = item->next;
        }
        list.clear();
    }

    // L06: TODO 7: Method to ask for the value of a custom property
    int GetProperty(const char* name, int defaultValue = 0) const;
    void SetProperty(const char* name, int value);

    List<Property*> list;
};

struct Tile
{
    int id;
    Properties properties;
};

struct TileSet
{
    int firstgId;
    SString name;
    int tileW, tileH;
    int spacing;
    int margin;

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

struct MapLayer
{
    SString	name;
    int width;
    int height;
    uint* data;

    Properties properties;

    MapLayer() : data(NULL)
    {}

    ~MapLayer()
    {
        RELEASE(data);
    }

    // Function to get the value of x and y
    inline uint Get(int x, int y) const
    {
        uint result = data[y * width + x];
        return result;
    }
};

struct MapData
{
    int w, h;
    int tileW, tileH;
    SDL_Color backgroundColor;
    MapTypes type;
    List<TileSet*> tileSets;
    List<MapLayer*> mapLayer;
};

class Map : public Module
{
public:

    Map();

    // Destructor
    virtual ~Map();

    void Init();

    // Called before render is available
    bool Awake(pugi::xml_node& conf);

    // Called each loop iteration
    void Draw();

    // Called before quitting
    bool CleanUp();

    // Load new map
    bool Load(const char* path);

    // Translates map position to world position
    iPoint MapToWorld(int x, int y) const;

    // Changes property to value assigned
    void SetTileProperty(int x, int y, const char* property, int value, bool notMovCollision = false, bool isObject = false);

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
};

#endif // __MAP_H__