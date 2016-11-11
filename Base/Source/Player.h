#ifndef _PLAYER_H
#define _PLAYER_H

#include "Mesh.h"
#include "Vector3.h"



class Player
{
private:
    Mesh* m_mesh;
    Vector3 m_pos;
    Vector3 m_scale;

    Vector3 m_cursorPos;
    double x, y;


public:

    Player();
    ~Player();

    void Init();
    void Update(float worldWidth, float worldHeight, double dt);

    void SetMesh(Mesh* mesh);
    Mesh* GetMesh();

    void SetPos(const Vector3& pos);
    Vector3 GetPos();

    void SetScale(const Vector3& scale);
    Vector3 GetScale();

    Vector3 GetCursorPos();

    void MovePlayer(int movedir, double dt);
};

#endif