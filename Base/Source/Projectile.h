#ifndef _PROJECTILE_H
#define _PROJECTILE_H

#include "GameObject.h"

class Projectile : public GameObject
{
public:
    Projectile();
    ~Projectile();

    virtual void Init();
    virtual void Update(double dt);

private:

};

#endif