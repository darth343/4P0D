#ifndef _PLAYER_H
#define _PLAYER_H

#include "Mesh.h"
#include "Vector3.h"
#include "Map.h"



class Player
{


public:

    enum ATTACK_TYPE
    {
        MELEE,
        RANGED,
    };

    Player();
    ~Player();

    void Init();
    void Update(double dt, CMap* m_cmap);

    void SetMesh(Mesh* mesh);
    Mesh* GetMesh();

    void SetPos(const Vector3& pos);
    Vector3 GetPos();

    void SetScale(const Vector3& scale);
    Vector3 GetScale();

    Vector3 GetCursorPos();

    void MovePlayer(int movedir, double dt);

    void Attack();

private:
    Mesh* m_mesh;
    Vector3 m_pos;
    Vector3 m_scale;

    Vector3 m_cursorPos;
    double x, y;
    int controllerID;

    float playerSpeed;

    ATTACK_TYPE m_attackType;

};

#endif