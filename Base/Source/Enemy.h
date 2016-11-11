#ifndef _ENEMY_H
#define _ENEMY_H

#include "GameObject.h"
#include "Projectile.h"
#include <vector>

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

    void SetEnemyType(ENEMY_TYPE type);
    ENEMY_TYPE GetEnemyType();

    void SetTarget(Vector3 target);
    Vector3 GetTarget();

    virtual void Init();
    virtual void Update(double dt, Vector3 newTarget);

    void Attack();
    void TakeDamage(int dmg);

    std::vector<Projectile*> m_ProjectileList;

private:
    int m_hp;
    ENEMY_TYPE m_enemyType;
    Vector3 m_target;
    double m_attackDelay;

};

#endif