#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector3.h"

struct GameObject
{
	enum GAMEOBJECT_TYPE
	{
		GO_NONE = 0,
		GO_BALL,
		GO_CUBE,
        GO_ASTEROID,    //asteroid
        GO_METEOR,      //meteor
        GO_SHIP,        //player ship
        GO_BULLET,      //player bullet
        GO_BLACKHOLE,   //player black hole skill
        GO_MISSILE,     //player missile
        GO_ENEMY_SHIP,  //enemy ship
        GO_ENEMY_BULLET,    //enemy bullet
        GO_DROP,    //drops from enemies
		GO_TOTAL, //must be last
	};

    enum DROP_TYPE
    {
        DROP_ADDLIFE,   //gain 1 life
        DROP_ADDBULLET, //gain an extra bullet
        DROP_BLACKHOLE, //gain 1 black hole to use
        DROP_TOTAL
    };

    enum ENEMY_TYPE
    {
        ENEMY_BOSS1,
        ENEMY_BOSS2,
        ENEMY_TOTAL
    };

	GAMEOBJECT_TYPE type;
	Vector3 pos;
	Vector3 vel;
	Vector3 scale;
    Vector3 direction;  //direction of movement
    Vector3 force;  //force for movement
	bool active;
	float mass;
    float angle;    //angle the object is rotated by
    float rotSpeed; //speed at which the object rotates
    int hp;     //hp of enemy objects
    DROP_TYPE droptype;     //type of drop
    GameObject* target;     //for missiles
    double shootTimeElapsed;
    double shootRate;
    ENEMY_TYPE bosstype;

	GameObject(GAMEOBJECT_TYPE typeValue = GO_BALL);
	~GameObject();

    void SetInactive();

    static int GO_COUNT;    //count of all the GameObjects
};

#endif