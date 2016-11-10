#ifndef _PROJECTILE_H
#define _PROJECTILE_H

#include "Vector3.h"
#include "Mesh.h"

class Projectile
{
    enum PROJECTILE_COLOUR
    {
        RED,
        GREEN,
        BLUE,
    };

public:
    Projectile();
    ~Projectile();

    void SetMesh(Mesh* mesh);
    Mesh* GetMesh();

    void SetPos(const Vector3& pos);
    Vector3 GetPos();

    void SetScale(const Vector3& scale);
    Vector3 GetScale();

    void SetSpeed(float speed);
    float GetSpeed();

    void SetColour(PROJECTILE_COLOUR colour);
    PROJECTILE_COLOUR GetColour();

    void Init();
    void Update(double dt);

private:

    Vector3 m_pos;
    Vector3 m_dir;
    Vector3 m_scale;
    float m_speed;
    PROJECTILE_COLOUR m_colour;
    Mesh* m_mesh;
};

#endif