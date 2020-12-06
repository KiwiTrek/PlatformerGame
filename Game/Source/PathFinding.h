#ifndef __PATHFINDING_H__
#define __PATHFINDING_H__

#include "Module.h"

#include "Point.h"
#include "DynArray.h"
#include "List.h"

#include "SDL.h"

#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255

class PathFinding : public Module
{
public:
	// Constructor
	PathFinding();

	// Destructor
	~PathFinding();

	// Called before the first frame
	bool Start();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before quitting
	bool CleanUp();

	// Sets up the walkability map
	void SetMap(uint width, uint height, uchar* data);

	// Main function to request a path from origin to destination
	int CreatePath(DynArray<iPoint>& path, const iPoint& origin, const iPoint& destination);

	// Returns true if pos is inside the map boundaries
	bool CheckBoundaries(const iPoint& pos) const;

	// Returns true if the tile is walkable
	bool IsWalkable(const iPoint& pos) const;

	// Returns the walkability value of a tile
	uchar GetTileCost(const iPoint& pos) const;

	// Draws the given path
	void DrawPath(DynArray<iPoint>* path);

private:
	// Walkability values of the whole map
	uchar* map;

	SString folderTexture;
	SDL_Texture* debugPath;
	uint width;
	uint height;
};

struct PathList;

// Struct to represent a node in the path creation
struct PathNode
{
	// Constructors
	PathNode();
	PathNode(int costSoFar, int heuristic, const iPoint& pos, const PathNode* parent);
	PathNode(const PathNode& node);

	// Fills a list of all valid adjacent pathnodes
	uint FindWalkableAdjacents(PathList& list_to_fill);

	// Calculates the score for a tile
	int Score() const;

	// Calculate the costSoFar and heuristic for a tile
	int CalculateTotalCost(const iPoint& destination);

	// Cost from origin to tile
	int costSoFar;

	// Cost from tile to destination
	int heuristic;

	// Position of the tile
	iPoint pos;

	// needed to reconstruct the path in the end
	const PathNode* parent;
};

// Struct to include a list of path nodes
struct PathList
{
	// Looks for a node in the list and returns it's list node or NULL
	const ListItem<PathNode>* Find(const iPoint& point) const;

	// Returns the Pathnode with lowest score in this list or NULL if empty
	ListItem<PathNode>* GetNodeLowestScore() const;

	// The list itself
	List<PathNode> list;
};

#endif // __PATHFINDING_H__