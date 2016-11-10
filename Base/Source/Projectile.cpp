#include "Projectile.h"

Projectile::Projectile()
{}

Projectile::~Projectile()
{}

void Projectile::SetMesh(Mesh* mesh)
{
    this->m_mesh = mesh;
}

Mesh* Projectile::GetMesh()
{
    return m_mesh;
}

void Projectile::SetPos(const Vector3& pos)
{
    this->m_pos = pos;
}

Vector3 Projectile::GetPos()
{
    return m_pos;
}

void Projectile::SetScale(const Vector3& scale)
{
    this->m_scale = scale;
}

Vector3 Projectile::GetScale()
{
    return m_scale;
}

void Projectile::SetSpeed(float speed)
{
    m_speed = speed;
}

float Projectile::GetSpeed()
{
    return m_speed;
}

void Projectile::SetColour(PROJECTILE_COLOUR colour)
{
    m_colour = colour;
}

Projectile::PROJECTILE_COLOUR Projectile::GetColour()
{
    return m_colour;
}

void Projectile::Init()
{

}

void Projectile::Update(double dt)
{
    m_pos += m_dir * m_speed;
}