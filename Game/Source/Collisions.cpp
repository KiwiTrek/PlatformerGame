#include "Collisions.h"

#include "App.h"

#include "Defs.h"
#include "Log.h"

Collisions::Collisions()
{
	name.Create("collisions");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		colliders[i] = nullptr;
	}

	matrix[Collider::Type::SOLID][Collider::Type::AIR] = false;
	matrix[Collider::Type::SOLID][Collider::Type::SOLID] = false;
	matrix[Collider::Type::SOLID][Collider::Type::SPIKE] = false;
	matrix[Collider::Type::SOLID][Collider::Type::PLAYER] = false;
	matrix[Collider::Type::SOLID][Collider::Type::FRUIT] = false;
	matrix[Collider::Type::SOLID][Collider::Type::COIN] = false;
	matrix[Collider::Type::SOLID][Collider::Type::CHECKPOINT] = false;
	matrix[Collider::Type::SOLID][Collider::Type::GOAL] = false;
	matrix[Collider::Type::SOLID][Collider::Type::ENEMY] = false;
	matrix[Collider::Type::SOLID][Collider::Type::ATTACK] = false;

	matrix[Collider::Type::SPIKE][Collider::Type::AIR] = false;
	matrix[Collider::Type::SPIKE][Collider::Type::SOLID] = false;
	matrix[Collider::Type::SPIKE][Collider::Type::SPIKE] = false;
	matrix[Collider::Type::SPIKE][Collider::Type::PLAYER] = false;
	matrix[Collider::Type::SPIKE][Collider::Type::FRUIT] = false;
	matrix[Collider::Type::SPIKE][Collider::Type::COIN] = false;
	matrix[Collider::Type::SPIKE][Collider::Type::CHECKPOINT] = false;
	matrix[Collider::Type::SPIKE][Collider::Type::GOAL] = false;
	matrix[Collider::Type::SPIKE][Collider::Type::ENEMY] = false;
	matrix[Collider::Type::SPIKE][Collider::Type::ATTACK] = false;

	matrix[Collider::Type::PLAYER][Collider::Type::AIR] = false;
	matrix[Collider::Type::PLAYER][Collider::Type::SOLID] = false;
	matrix[Collider::Type::PLAYER][Collider::Type::SPIKE] = false;
	matrix[Collider::Type::PLAYER][Collider::Type::PLAYER] = false;
	matrix[Collider::Type::PLAYER][Collider::Type::FRUIT] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::COIN] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::CHECKPOINT] = false;
	matrix[Collider::Type::PLAYER][Collider::Type::GOAL] = false;
	matrix[Collider::Type::PLAYER][Collider::Type::ENEMY] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::ATTACK] = true;

	matrix[Collider::Type::FRUIT][Collider::Type::AIR] = false;
	matrix[Collider::Type::FRUIT][Collider::Type::SOLID] = false;
	matrix[Collider::Type::FRUIT][Collider::Type::SPIKE] = false;
	matrix[Collider::Type::FRUIT][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::FRUIT][Collider::Type::FRUIT] = false;
	matrix[Collider::Type::FRUIT][Collider::Type::COIN] = false;
	matrix[Collider::Type::FRUIT][Collider::Type::CHECKPOINT] = false;
	matrix[Collider::Type::FRUIT][Collider::Type::GOAL] = false;
	matrix[Collider::Type::FRUIT][Collider::Type::ENEMY] = false;
	matrix[Collider::Type::FRUIT][Collider::Type::ATTACK] = false;

	matrix[Collider::Type::COIN][Collider::Type::AIR] = false;
	matrix[Collider::Type::COIN][Collider::Type::SOLID] = false;
	matrix[Collider::Type::COIN][Collider::Type::SPIKE] = false;
	matrix[Collider::Type::COIN][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::COIN][Collider::Type::FRUIT] = false;
	matrix[Collider::Type::COIN][Collider::Type::COIN] = false;
	matrix[Collider::Type::COIN][Collider::Type::CHECKPOINT] = false;
	matrix[Collider::Type::COIN][Collider::Type::GOAL] = false;
	matrix[Collider::Type::COIN][Collider::Type::ENEMY] = false;
	matrix[Collider::Type::COIN][Collider::Type::ATTACK] = false;

	matrix[Collider::Type::CHECKPOINT][Collider::Type::AIR] = false;
	matrix[Collider::Type::CHECKPOINT][Collider::Type::SOLID] = false;
	matrix[Collider::Type::CHECKPOINT][Collider::Type::SPIKE] = false;
	matrix[Collider::Type::CHECKPOINT][Collider::Type::PLAYER] = false;
	matrix[Collider::Type::CHECKPOINT][Collider::Type::FRUIT] = false;
	matrix[Collider::Type::CHECKPOINT][Collider::Type::COIN] = false;
	matrix[Collider::Type::CHECKPOINT][Collider::Type::CHECKPOINT] = false;
	matrix[Collider::Type::CHECKPOINT][Collider::Type::GOAL] = false;
	matrix[Collider::Type::CHECKPOINT][Collider::Type::ENEMY] = false;
	matrix[Collider::Type::CHECKPOINT][Collider::Type::ATTACK] = false;

	matrix[Collider::Type::GOAL][Collider::Type::AIR] = false;
	matrix[Collider::Type::GOAL][Collider::Type::SOLID] = false;
	matrix[Collider::Type::GOAL][Collider::Type::SPIKE] = false;
	matrix[Collider::Type::GOAL][Collider::Type::PLAYER] = false;
	matrix[Collider::Type::GOAL][Collider::Type::FRUIT] = false;
	matrix[Collider::Type::GOAL][Collider::Type::COIN] = false;
	matrix[Collider::Type::GOAL][Collider::Type::CHECKPOINT] = false;
	matrix[Collider::Type::GOAL][Collider::Type::GOAL] = false;
	matrix[Collider::Type::GOAL][Collider::Type::ENEMY] = false;
	matrix[Collider::Type::GOAL][Collider::Type::ATTACK] = false;

	matrix[Collider::Type::ENEMY][Collider::Type::AIR] = false;
	matrix[Collider::Type::ENEMY][Collider::Type::SOLID] = false;
	matrix[Collider::Type::ENEMY][Collider::Type::SPIKE] = false;
	matrix[Collider::Type::ENEMY][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::ENEMY][Collider::Type::FRUIT] = false;
	matrix[Collider::Type::ENEMY][Collider::Type::COIN] = false;
	matrix[Collider::Type::ENEMY][Collider::Type::CHECKPOINT] = false;
	matrix[Collider::Type::ENEMY][Collider::Type::GOAL] = false;
	matrix[Collider::Type::ENEMY][Collider::Type::ENEMY] = false;
	matrix[Collider::Type::ENEMY][Collider::Type::ATTACK] = true;

	matrix[Collider::Type::ATTACK][Collider::Type::AIR] = false;
	matrix[Collider::Type::ATTACK][Collider::Type::SOLID] = false;
	matrix[Collider::Type::ATTACK][Collider::Type::SPIKE] = false;
	matrix[Collider::Type::ATTACK][Collider::Type::PLAYER] = false;
	matrix[Collider::Type::ATTACK][Collider::Type::FRUIT] = false;
	matrix[Collider::Type::ATTACK][Collider::Type::COIN] = false;
	matrix[Collider::Type::ATTACK][Collider::Type::CHECKPOINT] = false;
	matrix[Collider::Type::ATTACK][Collider::Type::GOAL] = false;
	matrix[Collider::Type::ATTACK][Collider::Type::ENEMY] = true;
	matrix[Collider::Type::ATTACK][Collider::Type::ATTACK] = false;
}

Collisions::~Collisions()
{}

void Collisions::Init()
{
	active = false;
}

bool Collisions::Start()
{
	return true;
}

bool Collisions::Awake(pugi::xml_node&)
{
	return true;
}

bool Collisions::PreUpdate()
{
	// Delete colliders scheduled for deletion
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr && colliders[i]->pendingToDelete == true)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	Collider* c1;
	Collider* c2;

	// Check collisions
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		// skip empty colliders
		if (colliders[i] == nullptr)
		{
			continue;
		}

		c1 = colliders[i];

		// avoid checking collisions already checked
		for (uint k = i + 1; k < MAX_COLLIDERS; ++k)
		{
			// skip empty colliders
			if (colliders[k] == nullptr)
			{
				continue;
			}

			c2 = colliders[k];

			if (c1->Intersects(c2->rect) && matrix[c1->type][c2->type])
			{
				if (c1->listener)
				{
					c1->listener->OnCollision(c1, c2);
				}
				if (c2->listener)
				{
					c2->listener->OnCollision(c2, c1);
				}
			}
		}
	}

	return true;
}

bool Collisions::CleanUp()
{
	LOG("Freeing all colliders");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

bool Collisions::Load(pugi::xml_node&)
{
	return true;
}

bool Collisions::Save(pugi::xml_node&)
{
	return true;
}

Collider* Collisions::AddCollider(SDL_Rect rect, Collider::Type type, Module* listener)
{
	Collider* ret = nullptr;
	// Adds the collider in the first null spot on the array
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, listener);
			break;
		}
	}

	return ret;
}

void Collider::SetPos(int x, int y, int w, int h)
{
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
}

bool Collider::Intersects(const SDL_Rect& r) const
{
	return (rect.x < r.x + r.w && rect.x + rect.w > r.x && rect.y < r.y + r.h && rect.h + rect.y > r.y);
}