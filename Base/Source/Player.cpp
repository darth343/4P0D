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

void Player::Update()
{
    // Get the updated cursor positions from Application
    Application::GetCursorPos(&m_cursorPosX, &m_cursorPosY);
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

double Player::GetCursorPosX()
{
    return m_cursorPosX;
}

double Player::GetCursorPosY()
{
    return m_cursorPosY;
}