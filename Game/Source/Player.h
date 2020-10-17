#ifndef __MODULE_PLAYER_H__
#define __MODULE_PLAYER_H__

#include "Module.h"

class Player : public Module {
public:

	Player();

	// Destructor
	virtual ~Player();

	// Called before player is available
	//bool Awake(pugi::xml_node&);			<- do we need this? i think not

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&);

private:
};

#endif // !__MODULE_PLAYER_H__

