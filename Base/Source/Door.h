#ifndef _LOCKED_DOOR_H
#define _LOCKED_DOOR_H

#include "Switch.h"
#include "Map.h"

class Door : public GameObject
{
public:
    Door();
    ~Door();

    Switch* m_switch;

    void Init();
    void Update(double dt, CMap* spawnMap);
};

#endif