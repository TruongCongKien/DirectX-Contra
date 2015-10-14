#include "PlayerMBullet.h"
#include "PlayerMBulletMovingState.h"

PlayerMBullet :: PlayerMBullet(float x, float y, bool  isBoosting, float angle)
{
	pData = new SpriteData();
	
	pData ->ppTextureArrays = new TextureArray*[1];

	pData ->ppTextureArrays[0] = new TextureArray("Resources\\Sprites\\Bullets", "mbullet","", 1, 60, D3DCOLOR_XRGB(255,255,255));

	pData -> ppTextureArrays[0] ->setAnchorPoint( 0.5f,0.5f );

	pData -> x = x;
	
	pData -> y = y;

	pData -> body = RectF(- pData ->ppTextureArrays[0] ->getWidth() / 2, -pData ->ppTextureArrays[0] ->getHeight(),pData -> ppTextureArrays[0] ->getWidth() , pData ->ppTextureArrays[0] ->getHeight());
	
	pData -> pState = new PlayerMBulletMovingState(pData, angle);
}

void PlayerMBullet :: draw(Camera* cam)
{
	pData ->ppTextureArrays[0] ->draw(pData -> x, pData -> y, cam );
}
void PlayerMBullet :: update()
{
	pData ->pState ->onUpdate();
}