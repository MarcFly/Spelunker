#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "Entity.h"
#include "p2List.h"

class j1Player : public Entity
{
public:

	j1Player();


	// Called before render is available
	bool Awake(const pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called each loop iteration, for frame specific interactions
	bool UpdateTick(float dt);

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// OnCollision reaction
	void OnCollision(Collider* c1, Collider* c2);

public:

	
};


#endif // __j1TEXTURES_H__