#ifndef __j1Collisions_H__
#define __j1Collisions_H__


#define FARLANDS iPoint({5000, 5000})

#include "j1Module.h"
#include "p2Defs.h"
#include "Animation.h"
#include "p2List.h"

struct SDL_Rect;
enum COLLIDER_TYPE
{
	COLLIDER_NONE = 63,
	COLLIDER_PLAYER,
	//COLLIDER_WALL,
	COLLIDER_HOOK_RING,
	COLLIDER_HOOK_RANGE,
	COLLIDER_GROUND,
	COLLIDER_DIE,
	COLLIDER_PLAYER_AIR,
	COLLIDER_PLAYER_WALL,
	COLLIDER_END,
	
	COLLIDER_MAX
};

struct TypeRect {
	COLLIDER_TYPE type;
	SDL_Rect rect;
};

struct Collider {
	bool to_delete = false;
	COLLIDER_TYPE type;
	j1Module* callback = nullptr;

	bool active = true;

	SDL_Rect rect;

	Collider(SDL_Rect rect_, COLLIDER_TYPE type_, j1Module* callback_ = nullptr) {
		rect = rect_;
		type = type_;
		callback = callback_;
	}

	void SetPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}

	//Directions ColliderHit(const SDL_Rect& r) const;
	int CheckCollision(const SDL_Rect& r, SDL_Rect& res_rect) const;
	
	//bool FutureCheck(const SDL_Rect& r) const;
};


class j1Collision : public j1Module
{
public:

	j1Collision();
	~j1Collision();

	bool Awake(pugi::xml_node& config);
	bool PreUpdate();
	bool Update(float dt);
	bool CleanUp();

	Collider* AddCollider(iPoint pos, COLLIDER_TYPE type_, j1Module* callback_ = nullptr);
	bool EraseCollider(Collider* collider);
	void DebugDraw();

	SDL_Rect GetRectType(COLLIDER_TYPE type);

	bool debug = false;
	p2List<TypeRect*> rect_list;

private:

	p2List<Collider*> passive_colliders;
	p2List<Collider*> dynamic_colliders;
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
	
	
};

#endif // __j1Collisions_H__