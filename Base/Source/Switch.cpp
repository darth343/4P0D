#include "Switch.h"
#include "Projectile.h"
Switch::Switch()
{}

Switch::~Switch()
{}

void Switch::InteractionResponse(GameObject* OtherGo)
{
    switch (m_SwitchType)
    {
    case EITHER:
    {
                   if (OtherGo->GetType() == GameObject::PROJECTILE_MELEE || OtherGo->GetType() == GameObject::PROJECTILE_RANGED)
                   {
                       this->SetActive(false);
                       OtherGo->SetActive(false);
                   }
                   break;
    }

    case MELEE_ONLY:
    {
                   if (OtherGo->GetType() == GameObject::PROJECTILE_MELEE)
                   {
                       this->SetActive(false);
                       OtherGo->SetActive(false);
                   }
                   break;
    }

    case RANGED_ONLY:
    {
                   if (OtherGo->GetType() == GameObject::PROJECTILE_RANGED)
                   {
                       this->SetActive(false);
                       OtherGo->SetActive(false);
                   }
                   break;
    }
    }
}