
#include "GameObject.h"

int GameObject::GO_COUNT = 0;

GameObject::GameObject(GAMEOBJECT_TYPE typeValue) 
	: type(typeValue),
	scale(1, 1, 1),
	active(false),
	mass(1.f),
    angle(0.f),
    rotSpeed(0.f)
{
}

GameObject::~GameObject()
{
}

void GameObject::SetInactive()
{
    active = false;
    --GameObject::GO_COUNT;
}