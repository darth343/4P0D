#ifndef _SWITCH_H
#define _SWITCH_H

#include "GameObject.h"

class Switch : public GameObject
{
public:
    Switch();
     ~Switch();

    enum SWITCH_TYPE
    {
        EITHER,
        MELEE_ONLY,
        RANGED_ONLY,
    };

    SWITCH_TYPE m_SwitchType;

    void InteractionResponse(GameObject* OtherGo);

};

#endif