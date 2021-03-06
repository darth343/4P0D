#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector3.h"
#include "Mesh.h"
#include <string>
#include "AABB.h"
#include "Map.h"

class GameObject
{

public:
    enum GAMEOBJECT_TYPE
    {
        PROJECTILE_MELEE,
        PROJECTILE_RANGED,
        PLAYER,
        ENEMY,
        SWITCH,
        DOOR,
        SPAWNER,
    };

    ~GameObject();
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

    void SetType(GAMEOBJECT_TYPE type);
    GAMEOBJECT_TYPE GetType();

    void SetActive(bool status);
    bool GetActive();

    void SetInactive();

    bool CheckCollisionWith(GameObject* otherGo);

    void UpdateAABB();
    bool CheckCollisionWithAABB(GameObject* otherGo);

	bool CheckCollisionWithMap(CMap* m_cMap, CMap* spawnMap);

protected:
    GameObject();


    std::string m_name;
    OBJECT_COLOUR m_colour;
    Mesh* m_mesh;
    Vector3 m_pos;
    Vector3 m_scale;
    Vector3 m_velocity;
    bool m_active;

    GAMEOBJECT_TYPE m_type;

    AABB *m_Collider;

};

#endif