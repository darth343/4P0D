
#include "GameObject.h"

GameObject::GameObject()
: m_pos(Vector3(0, 0, 0))
, m_scale(Vector3(1, 1, 1))
, m_active(false)
{
    m_Collider = new AABB();
}

GameObject::~GameObject()
{
}

void GameObject::SetInactive()
{
    m_active = false;
}

void GameObject::SetMesh(Mesh* mesh)
{
    this->m_mesh = mesh;
}

Mesh* GameObject::GetMesh()
{
    return m_mesh;
}

void GameObject::SetPos(const Vector3& pos)
{
    this->m_pos = pos;
}

Vector3 GameObject::GetPos()
{
    return m_pos;
}

void GameObject::SetScale(const Vector3& scale)
{
    this->m_scale = scale;
}

Vector3 GameObject::GetScale()
{
    return m_scale;
}

void GameObject::SetColour(OBJECT_COLOUR colour)
{
    m_colour = colour;
}

GameObject::OBJECT_COLOUR GameObject::GetColour()
{
    return m_colour;
}

void GameObject::SetVelocity(Vector3 vel)
{
    m_velocity = vel;
}

Vector3 GameObject::GetVelocity()
{
    return m_velocity;
}

bool GameObject::CheckCollisionWith(GameObject* otherGo)
{
    float distSquare = (this->m_pos - otherGo->m_pos).LengthSquared();
    float combinedRadiusSquare = (this->m_scale.x + otherGo->m_scale.x) * (this->m_scale.y + otherGo->m_scale.y);

    if (distSquare <= combinedRadiusSquare) {
        return true;
    }
    return false;
}

bool GameObject::CheckCollisionWithAABB(GameObject* otherGo)
{
    if ((otherGo->m_pos.x < this->m_Collider->m_MaxPoint.x && otherGo->m_pos.x > this->m_Collider->m_MinPoint.x)
        && (otherGo->m_pos.y < this->m_Collider->m_MaxPoint.y && otherGo->m_pos.y > this->m_Collider->m_MinPoint.y))
    {
        return true;
    }
    return false;
}

bool GameObject::CheckCollisionWithMap(CMap* m_cMap, CMap* spawnMap)
{
	Vector3 nextTile = m_pos * (1.f / m_cMap->GetTileSize());
	Vector3 halfNextTile = (m_pos + Vector3((m_cMap->GetTileSize() / 2), (m_cMap->GetTileSize() / 2), 0.f)) * (1.f / m_cMap->GetTileSize());

	if (nextTile.y > 0)
	{
		if (m_cMap->theMap[nextTile.y][halfNextTile.x].CheckCollide()
			|| spawnMap->theMap[nextTile.y][halfNextTile.x].CheckCollide())
		{
			return true;
		}
	}
	else if (nextTile.y < 0)
	{
		if (m_cMap->theMap[nextTile.y + 2][nextTile.x].CheckCollide()
			|| spawnMap->theMap[nextTile.y + 2][nextTile.x].CheckCollide())
		{
			return true;
		}
	}

	if (nextTile.x > 0)
	{
		if (m_cMap->theMap[halfNextTile.y][nextTile.x].CheckCollide()
			|| spawnMap->theMap[halfNextTile.y][nextTile.x].CheckCollide())
		{
			return true;
		}
	}
	else if (nextTile.x < 0)
	{
		if (m_cMap->theMap[nextTile.y][nextTile.x].CheckCollide()
			|| spawnMap->theMap[nextTile.y][nextTile.x].CheckCollide())
		{
			return true;
		}
	}

	return false;
}

void GameObject::SetActive(bool status)
{
    m_active = status;
}

bool GameObject::GetActive()
{
    return m_active;
}

void GameObject::SetType(GAMEOBJECT_TYPE type)
{
    m_type = type;
}

GameObject::GAMEOBJECT_TYPE GameObject::GetType()
{
    return m_type;
}

void GameObject::Init()
{

}

void GameObject::Update(double dt)
{

}

void GameObject::UpdateAABB()
{
    m_Collider->Update(m_pos, m_scale);
}