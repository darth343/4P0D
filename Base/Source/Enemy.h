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
    };

    void SetHP(int hp);
    int GetHp();

    virtual void Init();
    virtual void Update(double dt);

private:
    int m_hp;
};

#endif