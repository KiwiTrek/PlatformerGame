#include "App.h"
#include "PathFinding.h"
#include "Map.h"
#include "Render.h"
#include "Textures.h"

#include "Defs.h"
#include "Log.h"

PathFinding::PathFinding() : Module(), map(NULL), width(0), height(0)
{
	name.Create("pathfinding");
}

PathFinding::~PathFinding()
{
	RELEASE_ARRAY(map);
}

bool PathFinding::Awake(pugi::xml_node& config)
{
	LOG("Loading PathFinder");
	bool ret = true;

	folderTexture.Create(config.child("folderTexture").child_value());

	return ret;
}

bool PathFinding::Start()
{
	SString tmp("%s%s", folderTexture.GetString(), "path_meta_data.png");
	debugPath = app->tex->Load(tmp.GetString());

	return true;
}

bool PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	RELEASE_ARRAY(map);

	return true;
}

void PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width * height];
	memcpy(map, data, width * height);
}

bool PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width && pos.y >= 0 && pos.y <= (int)height);
}

bool PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileCost(pos);
	return t != INVALID_WALK_CODE && t >= 0 && t != 254;
}

uchar PathFinding::GetTileCost(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
	{
		return map[(pos.y * width) + pos.x];
	}

	return INVALID_WALK_CODE;
}

void PathFinding::DrawPath(DynArray<iPoint>* path)
{
	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		app->render->DrawTexture(debugPath, pos.x, pos.y);
	}
}

const ListItem<PathNode>* PathList::Find(const iPoint& point) const
{
	ListItem<PathNode>* item = list.start;
	while (item)
	{
		if (item->data.pos == point)
		{
			return item;
		}
		item = item->next;
	}
	return NULL;
}

ListItem<PathNode>* PathList::GetNodeLowestScore() const
{
	ListItem<PathNode>* ret = NULL;
	int min = 65535;

	ListItem<PathNode>* item = list.end;
	while (item)
	{
		if (item->data.Score() < min)
		{
			min = item->data.Score();
			ret = item;
		}
		item = item->prev;
	}
	return ret;
}

PathNode::PathNode() : costSoFar(-1), heuristic(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int costSoFar, int heuristic, const iPoint& pos, const PathNode* parent) : costSoFar(costSoFar), heuristic(heuristic), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : costSoFar(node.costSoFar), heuristic(node.heuristic), pos(node.pos), parent(node.parent)
{}

uint PathNode::FindWalkableAdjacents(PathList& listToFill)
{
	iPoint cell;
	uint before = listToFill.list.Count();

	// north
	cell.Create(pos.x, pos.y + 1);
	if (app->pathfinding->IsWalkable(cell))
	{
		listToFill.list.Add(PathNode(-1, -1, cell, this));
	}

	// south
	cell.Create(pos.x, pos.y - 1);
	if (app->pathfinding->IsWalkable(cell))
	{
		listToFill.list.Add(PathNode(-1, -1, cell, this));
	}

	// east
	cell.Create(pos.x + 1, pos.y);
	if (app->pathfinding->IsWalkable(cell))
	{
		listToFill.list.Add(PathNode(-1, -1, cell, this));
	}

	// west
	cell.Create(pos.x - 1, pos.y);
	if (app->pathfinding->IsWalkable(cell))
	{
		listToFill.list.Add(PathNode(-1, -1, cell, this));
	}

	return listToFill.list.Count();
}

int PathNode::Score() const
{
	return costSoFar + heuristic;
}

int PathNode::CalculateTotalCost(const iPoint& destination)
{
	costSoFar = parent->costSoFar + 1;
	heuristic = pos.DistanceTo(destination);

	return costSoFar + heuristic;
}

int PathFinding::CreatePath(DynArray<iPoint>& path, const iPoint& origin, const iPoint& destination)
{
	if (!IsWalkable(origin) || !IsWalkable(destination))
	{
		return -1;
	}

	PathList open;
	PathList close;
	open.list.Add(PathNode(0, origin.DistanceTo(destination), origin, nullptr));
	while (open.list.Count() != 0)
	{
		close.list.Add(open.GetNodeLowestScore()->data);
		open.list.Del(open.GetNodeLowestScore());

		if (close.list.end->data.pos == destination)
		{
			int counter = 1;
			PathNode backtrack = close.list.end->data;
			path.PushBack(backtrack.pos);
			do
			{
				counter++;
				backtrack = close.Find(backtrack.parent->pos)->data;
				path.PushBack(backtrack.pos);
			} while (backtrack.parent != nullptr);
			path.Flip();
			return counter;
		}

		PathList adjNodes;
		close.list.end->data.FindWalkableAdjacents(adjNodes);
		for (ListItem<PathNode>* i = adjNodes.list.start; i != NULL; i = i->next)
		{
			if (close.Find(i->data.pos) != NULL)
			{
				continue;
			}
			else if (open.Find(i->data.pos) != NULL)
			{
				PathNode tmp = open.Find(i->data.pos)->data;
				i->data.CalculateTotalCost(destination);
				if (i->data.costSoFar < tmp.costSoFar)
				{
					tmp.parent = i->data.parent;
				}
			}
			else
			{
				i->data.CalculateTotalCost(destination);
				open.list.Add(i->data);
			}
		}
		adjNodes.list.Clear();
	}
	return -1;
}