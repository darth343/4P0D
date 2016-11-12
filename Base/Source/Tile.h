#ifndef TILE_H
#define TILE_H
#include "Vector3.h"

class Tile
{
public:
	Tile(Vector3 pos, int ID):
		Pos(pos),
		BlockID(ID),
		OpacityLevel(5),
        timer(0.f)
	{};
	Tile(){};
	~Tile(){};
	void SetCollision(bool collide)
	{
		shouldCollide = collide;
	}
	int OpacityLevel;
    bool CheckCollide()
    {
        if (active && shouldCollide)
            return true;
        else 
            return false;
    }
    float timer;
	int BlockID;
	Vector3 Pos;
	bool shouldCollide;
	void SetPosition();
	void onTrigger();
    bool active;
};

#endif