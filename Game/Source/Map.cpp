#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"

#include "Defs.h"
#include "Log.h"

#include <math.h>

Map::Map() : Module(), mapLoaded(false)
{
    name.Create("map");
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
    LOG("Loading Map Parser");
    bool ret = true;

    folder.Create(config.child("folder").child_value());

    return ret;
}

void Map::Draw()
{
    if (mapLoaded == false)
    {
        return;
    }

    //Spacing(?) WIP
    ListItem <Tileset*>* t = tilesets.start;
    while (t != NULL)
    {
        for (int j = 0; j < 6; ++j)
        {
            for (int i = 0; i < 8; ++i)
            {
                SString source;
                source.Create("Assets/maps/%s", t->data->imageSource.GetString());
                SDL_Texture* tilesetTexture = app->tex->Load(source.GetString());
                SDL_Rect section = { t->data->tileW * i + t->data->margin,t->data->tileH * j + t->data->margin,t->data->tileW,t->data->tileH };
                app->render->DrawTexture(tilesetTexture, t->data->tileW * i - t->data->spacing, t->data->tileH * j - t->data->spacing, false, &section);
            }
        }
        t = t->next;
    }
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    tilesets.clear();

    mapFile.reset();

    return true;
}

// Load new map
bool Map::Load(const char* filename)
{
    bool ret = true;
    SString tmp("%s%s", folder.GetString(), filename);

    pugi::xml_parse_result result = mapFile.load_file(tmp.GetString());

    if (result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", filename, result.description());
        ret = false;
    }

    if (ret == true)
    {
        pugi::xml_node mapData = mapFile.child("map");
        LoadMap(mapData);
    }

    if (ret == true)
    {
        for (pugi::xml_node tileset = mapFile.child("map").child("tileset"); tileset; tileset = tileset.next_sibling("tileset"))
        {
            tilesets.add(LoadTileset(tileset));
        }
    }


    if (ret == true)
    {
        LOG("Successfully parsed map XML file: %s", filename);
        LOG("width: %d  height: %d", map.w, map.h);
        LOG("tile_width: %d  tile_height: %d", map.tileW, map.tileH);
        LOG("---------------------------");
        ListItem <Tileset*>* t = tilesets.start;
        int i = 1;
        while (t != NULL)
        {
            LOG("Tileset %d ----", i);
            LOG("name: %s firstgid: %d", t->data->name.GetString(), t->data->firstgId);
            LOG("tile_width: %d  tile_height: %d", t->data->tileW, t->data->tileH);
            LOG("spacing: %d  margin: %d", t->data->spacing, t->data->margin);
            t = t->next;
            ++i;
        }
    }

    mapLoaded = ret;

    return ret;
}

void Map::LoadMap(pugi::xml_node& mapData)
{
    map.version = mapData.attribute("version").as_float();
    map.orientation = mapData.attribute("orientation").as_string();
    map.renderOrder = mapData.attribute("renderorder").as_string();
    map.w = mapData.attribute("width").as_int();
    map.h = mapData.attribute("height").as_int();
    map.tileW = mapData.attribute("tilewidth").as_int();
    map.tileH = mapData.attribute("tileheight").as_int();
    map.nextObjectId = mapData.attribute("nextobjectid").as_int();
}

Tileset* Map::LoadTileset(pugi::xml_node& tileSetData)
{
    Tileset* tileset = new Tileset;
    tileset->firstgId = tileSetData.attribute("firstgid").as_int();
    tileset->name = tileSetData.attribute("name").as_string();
    tileset->tileW = tileSetData.attribute("tilewidth").as_int();
    tileset->tileH = tileSetData.attribute("tileheight").as_int();
    tileset->spacing = tileSetData.attribute("spacing").as_int();
    tileset->margin = tileSetData.attribute("margin").as_int();

    tileset->imageSource = tileSetData.child("image").attribute("source").as_string();
    tileset->imageW = tileSetData.child("image").attribute("width").as_int();
    tileset->imageH = tileSetData.child("image").attribute("height").as_int();

    return tileset;
}