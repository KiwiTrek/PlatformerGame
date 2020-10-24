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

    ListItem <MapLayer*>* L;
    L = data.mapLayer.start;
    while (L != NULL) // Iterate for all layers
    {
        for (int j = 0; j < L->data->height; ++j) // Iterate for all rows
        {
            for (int i = 0; i < L->data->width; ++i) // Iterate for all collumns
            {
                uint u = L->data->Get(i, j);
                SDL_Rect n = data.tilesets.start->data->GetTileRect(u);
                iPoint pos = MapToWorld(i, j);
                app->render->DrawTexture(data.tilesets.start->data->texture, pos.x, pos.y, false, &n);
            }
        }
        L = L->next; // Next layer
    }
}

// Translates map position to world position
iPoint Map::MapToWorld(int x, int y) const
{
    iPoint ret;
    ret.x = x * data.tileW;
    ret.y = y * data.tileH;

    return ret;
}

// Get relative Tile rectangle
SDL_Rect Tileset::GetTileRect(int id) const
{
    SDL_Rect rect = { 0 };

    if (id == 0)
    {
        return rect;
    }

    iPoint p = { 0,this->margin };
    int targetId = this->firstgId;
    for (int j = 0; j < this->numTilesHeight; ++j)
    {
        p.x = this->spacing;
        for (int i = 0; i < this->numTilesWidth; ++i)
        {
            if (id == targetId)
            {
                return SDL_Rect({ p.x,p.y,this->tileW,this->tileH });
            }
            p.x += this->tileW + this->spacing;
            ++targetId;
        }
        p.y += this->tileH + this->spacing;
    }
    return rect;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");
    ListItem<Tileset*>* item;
    item = data.tilesets.start;

    while (item != NULL)
    {
        RELEASE(item->data);
        item = item->next;
    }
    data.tilesets.clear();

    // Remove all layers
    ListItem<MapLayer*>* layerList;
    layerList = data.mapLayer.start;

    while (layerList != NULL)
    {
        RELEASE(layerList->data);
        layerList = layerList->next;
    }
    data.mapLayer.clear();

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
        // Load map
        ret = LoadMap();

        // Load tileset
        for (pugi::xml_node tileset = mapFile.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
        {
            Tileset* set = new Tileset();
            if(ret == true)
            {
                ret = LoadTilesetDetails(tileset, set);
            }
            if(ret == true)
            {
                ret = LoadTilesetImage(tileset, set);
            }
            data.tilesets.add(set);
        }

        // Load layers
        for (pugi::xml_node layer = mapFile.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
        {
            MapLayer* set = new MapLayer();
            if (ret == true)
            {
                ret = LoadLayer(layer, set);
            }
            data.mapLayer.add(set);
        }
        LogInfo();
    }
    mapLoaded = ret;

    return ret;
}

bool Map::LoadMap()
{
    bool ret = true;
    pugi::xml_node map = mapFile.child("map");

    if (map == NULL)
    {
        LOG("Error parsing map xml file: Cannot find 'map' tag.");
        ret = false;
    }
    else
    {
        LOG("Filling map info");
        //data.version = map.attribute("version").as_string();
        SString strType(map.attribute("orientation").as_string());
        data.type = StrToMapType(strType);
        //data.renderorder = map.attribute("renderorder").as_string();
        data.w = map.attribute("width").as_int();
        data.h = map.attribute("height").as_int();
        data.tileW = map.attribute("tilewidth").as_int();
        data.tileH = map.attribute("tileheight").as_int();
        //data.nextObjectId = map.attribute("nextobjectid").as_int();
    }
    return ret;
}

bool Map::LoadTilesetDetails(pugi::xml_node& tileset_node, Tileset* set)
{
    bool ret = true;

    LOG("Filling TilesetDetails");
    set->firstgId = tileset_node.attribute("firstgid").as_int();
    set->name = tileset_node.attribute("name").as_string();
    set->tileW = tileset_node.attribute("tilewidth").as_int();
    set->tileH = tileset_node.attribute("tileheight").as_int();
    set->spacing = tileset_node.attribute("spacing").as_int();
    set->margin = tileset_node.attribute("margin").as_int();

    return ret;
}

bool Map::LoadTilesetImage(pugi::xml_node& tileset_node, Tileset* set)
{
    bool ret = true;
    pugi::xml_node image = tileset_node.child("image");

    if (image == NULL)
    {
        LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
        ret = false;
    }
    else
    {
        LOG("Filling TilesetDetails");
        SString tmp("%s%s", folder.GetString(), image.attribute("source").as_string());
        set->texture = app->tex->Load(tmp.GetString());
        set->texWidth = image.attribute("width").as_int();
        set->texHeight = image.attribute("height").as_int();

        set->numTilesWidth = set->texWidth / set->tileW;
        set->numTilesHeight = set->texHeight / set->tileH;
        set->offsetX = 0;
        set->offsetY = 0;
    }

    return ret;
}

bool Map::StoreId(pugi::xml_node& node, MapLayer* layer, int index)
{
    bool ret = true;

    layer->data[index] = node.attribute("gid").as_uint(0);

    return ret;
}

bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
    bool ret = true;

    layer->name = node.attribute("name").as_string();
    layer->width = node.attribute("width").as_int();
    layer->height = node.attribute("height").as_int();
    layer->data = new uint[(data.w * data.h * sizeof(uint))];
    memset(layer->data, 0, size_t(data.w * data.h * sizeof(uint)));
    pugi::xml_node gidNode;

    int i = 0;
    for (gidNode = node.child("data").child("tile"); gidNode && ret; gidNode = gidNode.next_sibling("tile"))
    {
        if (ret == true) ret = StoreId(gidNode, layer, i);
        ++i;
    }

    LOG("Layer <<%s>> has loaded %d tiles", layer->name.GetString(), i);
    return ret;
}

MapTypes operator++(MapTypes& mode)
{
    mode = static_cast<MapTypes>((mode + 1) % 4);
    return mode;
}

MapTypes Map::StrToMapType(SString s)
{
    SString StrType[4];
    StrType[0] = "unknown", StrType[1] = "orthogonal", StrType[2] = "isometric", StrType[3] = "staggered";
    MapTypes type = MapTypes::MAPTYPE_UNKNOWN;
    for (int i = 0; i < 4; ++i)
    {
        if (s == StrType[i])
        {
            return ++type;
        }
    }
    return MAPTYPE_UNKNOWN;
}

void Map::LogInfo()
{
    // LOG all the data loaded
    LOG("--------------------------------------------------------------------------");
    LOG("<< MAP DATA >>");
    LOG("Width=%d", data.w);
    LOG("Height=%d", data.h);
    LOG("TileWidth=%d", data.tileW);
    LOG("TileHeight=%d", data.tileH);
    LOG("<< END MAP DATA >>\n");

    ListItem<Tileset*>* infoList;
    infoList = data.tilesets.start;
    while (infoList != NULL)
    {
        LOG("<< TILESET >>");
        LOG("Name=%s", infoList->data->name.GetString());
        LOG("Firstgid=%d", infoList->data->firstgId);
        LOG("Margin=%d", infoList->data->margin);
        LOG("Spacing=%d", infoList->data->spacing);
        LOG("Tile_width=%d", infoList->data->tileW);
        LOG("Tile_height=%d", infoList->data->tileH);

        LOG("texWidth=%d", infoList->data->texWidth);
        LOG("texHeight=%d", infoList->data->texHeight);
        LOG("numTilesWidth=%d", infoList->data->numTilesWidth);
        LOG("numTilesHeight=%d", infoList->data->numTilesHeight);
        LOG("<< END TILESET >>\n");

        infoList = infoList->next;
    }

    // LOG the info for each loaded layer
    ListItem<MapLayer*>* layerList;
    layerList = data.mapLayer.start;
    while (layerList != NULL)
    {
        LOG("<< LAYER >>");
        LOG("Name=%s", layerList->data->name.GetString());
        LOG("Width=%d", layerList->data->width);
        LOG("Height=%d", layerList->data->height);
        LOG("<< END LAYER >>\n");
        layerList = layerList->next;
    }
    LOG("--------------------------------------------------------------------------");
}