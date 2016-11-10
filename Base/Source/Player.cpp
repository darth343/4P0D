#include "Player.h"
#include "Application.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Init()
{
}

void Player::Update(float worldWidth, float worldHeight)
{
    // Get the updated cursor positions from Application
    Application::GetCursorPos(&x, &y);

    m_cursorPos.Set(x / Application::GetWindowWidth() * worldWidth, 100.f - (y / Application::GetWindowHeight() * worldHeight), 1.f);
}

void Player::SetMesh(Mesh* mesh)
{
    this->m_mesh = mesh;
}

Mesh* Player::GetMesh()
{
    return m_mesh;
}

void Player::SetPos(const Vector3& pos)
{
    this->m_pos = pos;
}

Vector3 Player::GetPos()
{
    return m_pos;
}

void Player::SetScale(const Vector3& scale)
{
    this->m_scale = scale;
}

Vector3 Player::GetScale()
{
    return m_scale;
}

Vector3 Player::GetCursorPos()
{
    return m_cursorPos;
}