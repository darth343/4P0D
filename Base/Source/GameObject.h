#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector3.h"
#include "Mesh.h"
#include <string>

class GameObject
{

public:

    enum OBJECT_COLOUR
    {
        RED,
        GREEN,
        BLUE,
    };

    virtual void Init();
    virtual void Update(double dt);

    void SetMesh(Mesh* mesh);
    Mesh* GetMesh();

    void SetPos(const Vector3& pos);
    Vector3 GetPos();

    void SetScale(const Vector3& scale);
    Vector3 GetScale();

    void SetVelocity(Vector3 m_velocity);
    Vector3 GetVelocity();

    void SetColour(OBJECT_COLOUR colour);
    OBJECT_COLOUR GetColour();

    void SetVelocity(Vector3 vel);
    Vector3 GetVelocity();

    void SetInactive();

    bool CheckCollisionWith(GameObject* otherGo);

protected:
    GameObject();
    ~GameObject();

    std::string m_name;
    OBJECT_COLOUR m_colour;
    Mesh* m_mesh;
    Vector3 m_pos;
    Vector3 m_scale;
    Vector3 m_velocity;
    bool m_active;

};

#endif