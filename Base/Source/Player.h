#ifndef _PLAYER_H
#define _PLAYER_H

#include "Mesh.h"
#include "Vector3.h"
#include "Projectile.h"
#include "Map.h"
#include <vector>

class Player
{
public:

    enum ATTACK_TYPE
    {
        MELEE,
        RANGED,
    };

	Vector3 m_dir;

    Player();
    ~Player();

    void Init();
    void Update(double dt, CMap* m_cMap, CMap* spawnMap);

    void SetMesh(Mesh* mesh);
    Mesh* GetMesh();

    void SetPos(const Vector3& pos);
    Vector3 GetPos();

    void SetScale(const Vector3& scale);
    Vector3 GetScale();

    void SetHP(float hp);
    float GetHP();

    Vector3 GetCursorPos();

    void MovePlayer(double dt, CMap* m_cMap, CMap* spawnMap);

    void Attack();

    void TakeDamage(int i);

    static std::vector<Projectile*> m_ProjectileList;
    int controllerID;
    ATTACK_TYPE m_attackType;
    static int points;
    static std::vector<Tile*> TilesToUpdateList;
    static bool recoverFog;
private:
	Vector3 prevHeroTile;
    Mesh* m_mesh;
    Vector3 m_pos;
    Vector3 m_scale;

    Vector3 m_cursorPos;
    double x, y;


    float playerSpeed;


    double m_attackDelay;

    float m_hp; 

};
#endif
