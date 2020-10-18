#ifndef __MAP_H__
#define __MAP_H__

#include "Module.h"
#include "List.h"
#include "Point.h"

#include "PugiXml\src\pugixml.hpp"

struct Tileset
{
    int firstgId;
    SString name;
    int tileW, tileH;
    int spacing;
    int margin;

    SString imageSource;
    int imageW, imageH;
};

struct MapNode
{
    float version;
    SString orientation;
    SString renderOrder;
    int w, h;
    int tileW, tileH;
    int nextObjectId;
};

class Map : public Module
{
public:

    Map();

    // Destructor
    virtual ~Map();

    // Called before render is available
    bool Awake(pugi::xml_node& conf);

    // Called each loop iteration
    void Draw();

    // Called before quitting
    bool CleanUp();

    // Load new map
    bool Load(const char* path);

    MapNode map;
    List<Tileset*> tilesets;

private:

    void LoadMap(pugi::xml_node& mapData);
    Tileset* LoadTileset(pugi::xml_node& tileSetData);

    pugi::xml_document mapFile;
    SString folder;
    bool mapLoaded;
};

#endif // __MAP_H__