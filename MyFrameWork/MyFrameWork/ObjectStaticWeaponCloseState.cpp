#include "ObjectStaticWeaponCloseState.h"


ObjectStaticWeaponCloseState::ObjectStaticWeaponCloseState(ObjectData *pData)
{
    this->pData = pData;
    this->pData->iCurrentArr = ObjectStaticWeaponData::CLOSED;
    this->pData->isHittable = true;
    this->pData->isPlayerCollisionable = false;
    frameDelay = 85;
    currentFramecount = 0;
}


ObjectStaticWeaponCloseState::~ObjectStaticWeaponCloseState()
{
}

void ObjectStaticWeaponCloseState::onUpdate()
{
    ObjectState::onUpdate();

    if (currentFramecount >= frameDelay)
    {
        if (this->pData->playerX - this->pData->x >= -OBJECT_STATIC_WEAPON_RANGE_OPEN)
        {
            this->pData->pState = new ObjectStaticWeaponPreparingState(this->pData, true);
        }        
    }
    else
    {
        currentFramecount++;
    }
}