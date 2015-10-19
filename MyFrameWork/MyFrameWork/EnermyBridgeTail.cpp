#include "EnermyBridgeTail.h"


EnermyBridgeTail::EnermyBridgeTail(float x, float y)
{
    pData = new EnermyBridgeData();
    pData->x = x;
    pData->y = y;
    pData->dir = Direction::createRight();
    pData->vx = pData->vy = 0;

    this->initTextureArrays(EnermyBridgeData::COUNT);
    this->addTextureArray(EnermyBridgeData::ALIVE, "tail", 3, 10);
    this->addTextureArray(EnermyBridgeData::EXPLOSION, "explosion", EnermyBridgeData::EXPLOSION_TEXTURE_COUNT, EnermyBridgeData::EXPLOSION_FRAMES_TIME);
    //this->pData->ppTextureArrays[EnermyBridgeData::EXPLOSION] = new TextureArray("Resources\\Sprites", "explosion", "", 3, 10);

    this->pData->ppTextureArrays[EnermyBridgeData::ALIVE]->setAnchorPoint(0.0, 0.0);
    this->pData->ppTextureArrays[EnermyBridgeData::EXPLOSION]->setAnchorPoint(0.0, 0.0);
    this->pData->body = RectF(0, 0, 32.0f, 32.0f);
    this->pData->pState = new EnermyBridgeAliveState(this->pData);
}


EnermyBridgeTail::~EnermyBridgeTail()
{
}
