#ifndef _PROJECTILE_H
#define _PROJECTILE_H

#include "GameObject.h"

class Projectile : public GameObject
{
public:
    Projectile();
    ~Projectile();

    void SetDmg(int dmg);
    int GetDmg();

    virtual void Init();
    virtual void Update(double dt);
private:
    int m_dmg;


};

#endif