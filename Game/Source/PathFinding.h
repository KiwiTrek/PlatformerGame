#ifndef __PATHFINDING_H__
#define __PATHFINDING_H__

#include "Module.h"

#include "Point.h"
#include "DynArray.h"
#include "List.h"

#define INVALID_WALK_CODE 255

class PathFinding
{
public:

	// NOTE: Constructor is private to avoid new instances creation

	// Get unique instance of the class
	static PathFinding* GetInstance();

	// Destructor
	~PathFinding();

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

private:
	// Singleton instance
	static PathFinding* instance;

	// Private constructor, alternatively: PathFinding()=delete;
	PathFinding();

	// Declare the copy constructor and the assignment operator as 
	// private (or delete them explicitly) to prevent cloning your object
	PathFinding(const PathFinding&);
	PathFinding& operator=(const PathFinding&);

	// Size of the map
	uint width;
	uint height;

	// Walkability values of the whole map
	uchar* map;
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
	uint FindWalkableAdjacents(PathFinding* path, PathList& list_to_fill);

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