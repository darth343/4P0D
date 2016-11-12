#ifndef _ENEMY_H
#define _ENEMY_H

#include "GameObject.h"
#include "Projectile.h"
#include "Pathfinder.h"
#include "Player.h"
#include "Map.h"
#include <vector>
#include "Emitter.h"

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
    virtual void Update(double dt, Player* thePlayer, Player* otherPlay, CMap* m_cMap);
    void MoveToPlayer(double dt, Player* thePlayer, CMap* m_cMap);
	void MoveTo(double dt, Tile nextTile, int TileSize);
    void Attack();
    void TakeDamage(int dmg);

    std::vector<Projectile*> m_ProjectileList;

    Emitter* m_Emitter;

private:
	static Vector3 prevPlayerTile;
	float speed;
    int m_hp;
    ENEMY_TYPE m_enemyType;
	Vector3 m_target;
	Pathfinder pathfinder;
    double m_attackDelay;
    double findTargetDelay;

};

#endif