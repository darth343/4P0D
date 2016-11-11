#ifndef _ENEMY_H
#define _ENEMY_H

#include "GameObject.h"

class Enemy : public GameObject
{
public:
    Enemy();
    ~Enemy();

    enum ENEMY_TYPE
    {
        NORMAL,
        MELEE,
        RANGED,
    };

    void SetHP(int hp);
    int GetHp();

    void SetType(ENEMY_TYPE type);
    ENEMY_TYPE GetType();

    virtual void Init();
    virtual void Update(double dt);

private:
    int m_hp;
    ENEMY_TYPE m_type;
};

#endif