#include "Door.h"

Door::Door()
{}

Door::~Door()
{}

void Door::Init()
{
}

void Door::Update(double dt)
{
    if (!m_switch->GetActive())
        this->SetActive(false);
}