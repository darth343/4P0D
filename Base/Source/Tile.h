#ifndef TILE_H
#define TILE_H
#include "Vector3.h"

class Tile
{
public:
	Tile(Vector3 pos, int ID):
		Pos(pos),
		BlockID(ID),
		OpacityLevel(5)
	{};
	Tile(){};
	~Tile(){};
	void SetCollision(bool collide)
	{
		shouldCollide = collide;
	}
	int OpacityLevel;
	int BlockID;
	Vector3 Pos;
	bool shouldCollide;
	void SetPosition();
	void onTrigger();
};

#endif