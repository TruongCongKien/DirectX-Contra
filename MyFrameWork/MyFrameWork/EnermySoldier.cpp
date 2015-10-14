#include "EnermySoldier.h"


EnermySoldier::EnermySoldier(float x, float y)
{
    pData = new EnermySoldierData();

    pData->x = x;
    pData->y = y;
    
    this->initTextureArrays(EnermySoldierData::COUNT);
    this->addTextureArray(EnermySoldierData::RUN, "run", 3, 6); // them 1 state voi animation
    this->addTextureArray(EnermySoldierData::JUMP, "jump", 1, 60);
    this->addTextureArray(EnermySoldierData::DIE, "jump", 1, 60);
    this->pData->ppTextureArrays[EnermySoldierData::DESTROY] = new TextureArray("Resources\\Sprites", "explosion", "", 3, 10);
    this->pData->ppTextureArrays[EnermySoldierData::DESTROY]->setAnchorPoint(0.5f, 0.5f);

    pData->dir = Direction::createLeft();
    pData->body = RectF(-8.5f, -32.0f, 17.0f, 32.0f);
    pData->vx = pData->vy = 0;
    //pData->pState = new EnermySoldierRunningState(pData);
    //pData->pState = new EnermySoldierJumpingState(pData);
    //pData->pState = new EnermySoldierDestroyState(pData);
    pData->pState = new EnermySoldierDieState(pData);
}

EnermySoldier::~EnermySoldier()
{

}