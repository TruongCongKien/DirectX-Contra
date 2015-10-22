#include "Map.h"
#include <algorithm> 

Map :: Map(std::string mapName)
	:
	mapName(mapName)
{
	std::string convertFileName = std::string("Resources\\Maps\\") + (mapName) + std::string(".tmx");
	TiXmlDocument levelDocument;
	levelDocument.LoadFile(convertFileName);
	TiXmlElement* pRoot = levelDocument.RootElement();
	pRoot -> Attribute("tilewidth", &tileSize);
	pRoot ->Attribute("width", &width);
	pRoot -> Attribute ("height", &height);

	// load tile set
	for (TiXmlElement* e = pRoot ->FirstChildElement(); e != NULL; e = e-> NextSiblingElement())
	{
		if( e -> Value() == std :: string("tileset") )
		{
			loadTileSet(e);
		}
	}


	// load Layer

	for (TiXmlElement* e = pRoot ->FirstChildElement(); e != NULL; e = e-> NextSiblingElement())
	{
		if( e -> Value() == std :: string("layer"))
		{
			loadLayer(e);
		}
	}

	// create tileTmx in BackGroundLayer
	for (int i = 0; i < layers.size(); i++)
	{
		if(layers[i] ->getName() == "BackGroundLayer")
			layers[i] ->createTileTmx();
	}

	// Load Collision Rect
	for (TiXmlElement* e = pRoot ->FirstChildElement(); e != NULL; e = e-> NextSiblingElement())
	{
		if( e -> Value() == std :: string("objectgroup") && e ->Attribute("name") == std :: string("CollisionRect") )
		{
			loadCollisionRect(e);
		}
	}

	for (TiXmlElement* e = pRoot ->FirstChildElement(); e != NULL; e = e-> NextSiblingElement())
	{
		if( e -> Value() == std :: string("objectgroup") && e ->Attribute("name") == std :: string("ObjectLayer") )
		{
			loadObject(e);
		}
	}



	// create quadtree

	createMapCollsionTree();



}

#pragma region 


void Map :: loadTileSet(TiXmlElement* e)
	{
		TileSet* tileSet;
		tileSet = new TileSet();
		std::string s = std::string ("Resources\\Maps\\" ) + (e ->FirstChildElement()->Attribute("source"));

		std::wstring tileSetSource(s.begin(), s.end());

		tileSet ->name = e -> Attribute("name");

		Graphics::getInstance() ->loadTexture(s, e -> Attribute("name"));
		tileSet ->pTexture = Graphics::getInstance()->getTexture(tileSet->name) ->pTexture;
			
		e -> FirstChildElement() -> Attribute ("width", &tileSet ->width);
		e -> FirstChildElement() -> Attribute( "height", &tileSet -> height);
		e -> Attribute("firstgid",&tileSet->firstGridID);
		e -> Attribute("tilewidth", &tileSet -> tileWidth);
		e -> Attribute("tileheight", &tileSet -> tileHeight);
		e -> Attribute("spacing", &tileSet -> spacing);
		e -> Attribute("margin", &tileSet ->margin);
		


		int nCol = tileSet -> width / (tileSet -> tileWidth + tileSet ->spacing);
		if ( tileSet -> width % (tileSet -> tileWidth + tileSet ->spacing) > tileSet ->tileWidth )
		{
			nCol ++;
		}
		int nRow = tileSet -> height / (tileSet -> tileHeight + tileSet ->spacing);
		if ( tileSet -> tileHeight % (tileSet -> tileHeight + tileSet ->spacing) > tileSet ->tileHeight )
		{
			nRow ++;
		}
		
		tileSet -> nColumns = nCol;
		tileSet ->nRows = nRow;
		tileSet -> lastGridID = nCol * nRow + tileSet ->firstGridID -1;
		tileSets.push_back(tileSet);
	}

void Map :: loadLayer(TiXmlElement* pTileElement)
	{

		TileLayer* pTileLayer = new TileLayer(tileSize, width, height, pTileElement ->Attribute("name"),tileSets);

		std::vector<std::vector<int>> data;

		std::string decodedIDs;

		TiXmlElement* pDataNode = NULL;

		for (TiXmlElement* e = pTileElement->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
		{
			if (e->Value() == std::string("data"))
			{
				pDataNode = e;
			}		
		}

		for (TiXmlNode* e = pDataNode->FirstChild(); e != NULL; e = e->NextSibling())
		{
			TiXmlText* text = e->ToText();

			std::string t = text->Value();

			decodedIDs = base64_decode(t);
		}

		// uncompress zlib compression
		uLongf numGids = width * height * sizeof(int);

		std::vector< int > gids(width * height); 

		uncompress((Bytef*)&gids[0], &numGids, (const Bytef*)decodedIDs.c_str(), decodedIDs.size());

		std::vector<int> layerRow(width);

		for (int j = 0; j < height; j++)
		{
			data.push_back(layerRow);
		}
		for (int rows = 0; rows < height; rows++)
		{
			for (int cols = 0; cols < width; cols++)
			{
				data[rows][cols] = gids[rows * width + cols];
			}
		}

		pTileLayer->setTileIDs(data);

		layers.push_back(pTileLayer);
	}

void Map :: loadCollisionRect(TiXmlElement* pElement)
{
	RectI r;
	std::string type;

	for (TiXmlElement* e = pElement -> FirstChildElement(); e != NULL ; e = e -> NextSiblingElement())
	{
		e -> Attribute("x",&r.x);
		e -> Attribute("y",&r.y);
		e -> Attribute("width",&r.width);
		e -> Attribute("height",&r.height);
		type = e->Attribute("type");
		//collisionRects.push_back(r);
		collisionRectFs.push_back(CollisionRectF(r, type));
	}
}

void Map :: loadObject(TiXmlElement* pElement)
{
	//RectI r;

	int id;
	int x;
	int y;
	int width;
	int height;
	std:: string name;
	std:: string type;
	for (TiXmlElement* e = pElement -> FirstChildElement(); e != NULL ; e = e -> NextSiblingElement())
	{
		name = e -> Attribute("name");
		type = e -> Attribute("type");
		e -> Attribute("id",&id);
		e -> Attribute("x",&x);
		e -> Attribute("y",&y);
		e -> Attribute("width",&width);
		e -> Attribute("height",&height);

		if(type == "object")
		{
			x += width / 2;
			y += height;
		}
		Object* object = new Object(name, type, x , y , id , width, height, RectF(x , y , width, height));
		Objects.push_back(object);
	}
}

void Map :: createMapCollsionTree()
{
	mapCollisionTree = new QuadTree(0,RectF(0.0f, 0.0f , tileSize * width , tileSize * height));
}

void Map :: cleanMap(Camera* cam, PlayerSprite* sprite)
{
	// clean Enermy
	bool isOver = false;
	RectF camRect(cam ->getX(), cam ->getY(), cam -> getWidth(), cam -> getHeight());
	for (std::map < int, EnermySprite* > ::iterator it = enermyMap.begin(); it != enermyMap.end(); )
	{
		EnermySprite* temp = it -> second;
		RectF SpriteRect(temp ->getBody());
		if (!camRect.checkCollision(SpriteRect) || it ->second ->isDesTroyed())
		{
			if(it -> second ->getName().find("final") != std::string::npos)
			{
				isOver = true;
			}
			delete temp;
			it = enermyMap.erase(it);
		}
		else
		it++;
	}

	for (std::map < int, EnermySprite* > ::iterator it = enermyMap.begin(); it != enermyMap.end(); it ++)
	{
		std::vector < BulletSprite* > & enermyBullets = it ->second ->getBullets();
		for (int i = 0; i < enermyBullets.size(); i++)
		{
			Sprite* temp = enermyBullets[i];
			if(!camRect.checkCollision(temp  ->getBody()) )
			{
				delete temp;
				enermyBullets.erase(enermyBullets.begin() + i);
			}
		}
	}

	// clean ObjectSprite

	for (std :: map < int , ObjectSprite* > ::iterator it = objectMap.begin(); it != objectMap.end(); )
	{
		ObjectSprite* temp = it -> second;
		RectF SpriteRect(temp ->getBody());

		if (!camRect.checkCollision(SpriteRect) || it ->second ->isDesTroyed())
		{
			
			delete temp;
			it = objectMap.erase(it);
		}
		else
		it++;
	}

	


	if(isOver )
	{
		for (std::map < int, EnermySprite* > ::iterator it = enermyMap.begin(); it != enermyMap.end(); it ++)
		{
			it ->second ->die();
		}
	}

	std::vector<BulletSprite* >& playerBullets = sprite ->getBullets();

	for (int i = 0; i < playerBullets.size(); i++)
	{
		Sprite* temp = playerBullets[i];
		if(!camRect.checkCollision(temp  ->getBody()) )
		{
			delete temp;
			playerBullets.erase(playerBullets.begin() + i);
		}
	}

	


}

void Map ::addEToMap ( Camera* cam )
{
	RectF camRect = cam ->getRect();
	mapCollisionTree -> clear();
	for (int i = 0; i < Objects.size(); i++)
	{

			if( enermyMap.find(Objects[i] ->id) == enermyMap.end())
				mapCollisionTree -> insert( Objects[i] );
		 
  	}
 	std:: vector < Object * > returnList;
	mapCollisionTree -> getObjectlist (returnList, camRect);

	for (int i = 0; i < returnList.size(); i++)
	{
		RectF body = returnList[i] -> body;
		std::string type  = returnList[i] -> type;
		if(camRect.checkCollision( body ))
		{
			if(
				( ( body.x >= camRect.x - body.width ) && (body.x <= camRect.x - body.width / 2) )
				||
				( (body.x + body.width <= camRect.x +camRect.width + body.width) && ( body.x + body.width >= camRect.x +camRect.width + body.width / 2))
				)
			{
				if(type == "object")
				{
					objectMap[returnList[i] ->id ] = EnermyCreator :: getInstance() ->createObjectSprite( returnList[i] ->name, returnList[i]->x, returnList[i] -> y);
				}
				else
				{
					enermyMap[returnList[i] ->id] = EnermyCreator::getInstance() ->createEnermySprite(returnList[i] ->name, returnList[i]->x, returnList[i] -> y );	
				}
				
			}
			
		}
	}
}

#pragma endregion helperFunction

#pragma region


void Map :: draw(Camera* cam)
{
	for (int i = 0; i < layers.size(); i++)
	{
		if(layers[i] ->getName() == "BackGroundLayer")
			layers[i] ->draw(cam);
	}
	int fuck = enermyMap.size();
	for (std::map < int, EnermySprite* > ::iterator it = enermyMap.begin(); it != enermyMap.end(); ++it)
	{
		it ->second ->draw(cam);
	}

}



void Map :: onCollision(PlayerSprite* sprite, Camera* cam)
{
	// map collision vs Player
	mapCollisionTree -> clear();
	
	for (int i = 0; i < collisionRectFs.size(); i++)
	{
		mapCollisionTree -> insert( collisionRectFs[i] );
  	}
#pragma region
	
	// player vs Collision Rect
	std:: vector < CollisionRectF > returnList;
	mapCollisionTree -> getObjectlist (returnList, sprite -> getBody());


	for (int i = 0; i < returnList.size(); i++)
	{
		std:: vector < CollisionRectF > throughRectVector = sprite ->getThroughRect();
		if(std::find(throughRectVector.begin(), throughRectVector.end(), returnList[i]) == throughRectVector.end())
		{
			if( sprite ->getBody().checkCollision( returnList[i].rect) )
			{
				sprite -> onCollision( returnList[i] );
			}
		}
		
	}

	// player vs ObjectSprite


	for (std:: map < int ,ObjectSprite* > :: iterator objectIt = objectMap.begin() ; objectIt != objectMap.end(); objectIt++)
	{
		if(objectIt ->second ->isPlayerCollisionable())
		{
			CollisionRectF r = objectIt ->second ->getCollisionRect();
			if( sprite ->getBody().checkCollision(r.rect)  )
			{
				
				if( objectIt ->second ->isAffectble() )
							sprite -> onCollision( r );
				else
				{
					// Object dan vs cau ... :D
				}
			}
		}	
		
	}

#pragma endregion mapvsPlayer

#pragma region
	for (std::map < int, EnermySprite* > ::iterator it = enermyMap.begin(); it != enermyMap.end(); it++ )
	{
		RectF r = it ->second ->getBody();
		if(sprite ->isHittable() && r.checkCollision(sprite ->getBody()) )
		{
			sprite ->die();
		}
			
	}
#pragma endregion EnermyvsPlayer

#pragma region 

	std :: vector < BulletSprite* >& bullets = sprite ->getBullets();
	for (int bulletIt  = 0; bulletIt < bullets.size();  bulletIt++)
	{
		for (std::map < int, EnermySprite* > ::iterator enermyIt = enermyMap.begin(); enermyIt != enermyMap.end();)
		{
			Sprite* tempB = bullets[bulletIt];
			EnermySprite* tempE = enermyIt ->second;
			RectF rB = tempB ->getBody();
			RectF rE = tempE ->getBody();
			if( rE.checkCollision(rB ) && enermyIt ->second ->isHittable() )
			{
				tempE -> beShooted( sprite ->getDamage() );
				delete tempB;
				bullets.erase(bullets.begin() + bulletIt);
				break;
			}
			enermyIt++;
		}
	}

#pragma endregion playerBulletvsEnermy

#pragma region

	for (std::map < int, EnermySprite* > ::iterator enermyIt = enermyMap.begin(); enermyIt != enermyMap.end(); enermyIt++)
	{
		std:: vector < CollisionRectF > returnLists;
		mapCollisionTree -> getObjectlist (returnLists, enermyIt -> second ->getBody());

		for (int i = 0; i < returnLists.size(); i++)
		{

			std:: vector < CollisionRectF > throughRectVector = enermyIt ->second ->getThroughRect();
			if(std::find(throughRectVector.begin(), throughRectVector.end(), returnLists[i]) == throughRectVector.end())
			{
				if( enermyIt -> second ->getBody().checkCollision( returnLists[i].rect) )
				{
					enermyIt -> second -> onCollision( returnLists[i] );
				}
			}
			
		}


		for (std:: map < int ,ObjectSprite* > :: iterator objectIt = objectMap.begin() ; objectIt != objectMap.end(); objectIt++)
		{
			if(objectIt ->second ->isEnemyCollisionable())
			{
				CollisionRectF r = objectIt ->second ->getCollisionRect();
				if( sprite ->getBody().checkCollision(r.rect)  )
				{
				
					if( objectIt ->second ->isAffectble() )
								enermyIt -> second -> onCollision( r );
				}
			}	
		
		}



	}
#pragma endregion EnermyvsMap

#pragma region
	for (std::map < int, EnermySprite* > ::iterator enermyIt = enermyMap.begin(); enermyIt != enermyMap.end(); enermyIt++)
	{
		std :: vector < BulletSprite* >& bullets = enermyIt ->second ->getBullets();
		for (int i = 0; i < bullets.size(); i++)
		{
			Sprite* bulletE = bullets[i];
			if( sprite -> isHittable() && bulletE ->getBody().checkCollision(sprite ->getBody()) )
			{
				sprite -> die();
				bulletE -> die();
			}
		}		
	}
#pragma endregion EnermyBulletvsPlayer

#pragma region
	RectF r = cam ->getRect();
	sprite -> onCameraCollision(r);
#pragma endregion playervsCamera

#pragma region 

	for (int bulletIt  = 0; bulletIt < bullets.size();  bulletIt++)
	{
		for (std::map < int, ObjectSprite* > ::iterator objectIt = objectMap.begin(); objectIt != objectMap.end();)
		{
			Sprite* tempB = bullets[bulletIt];
			ObjectSprite* tempE = objectIt ->second;
			RectF rB = tempB ->getBody();
			RectF rE = tempE ->getBody();
			if( rE.checkCollision(rB ) && objectIt ->second ->isHittable() )
			{
				tempE -> beShooted( sprite ->getDamage() );
				delete tempB;
				bullets.erase(bullets.begin() + bulletIt);
				break;
			}
			objectIt++;
		}
	}

#pragma endregion playerBulletvsObjetSprite

	// update through rect list of player
	sprite ->updateThroughRect();
	// update through rect list of enermy
	for (std::map < int, EnermySprite* > ::iterator it = enermyMap.begin(); it != enermyMap.end(); it++)
	{
		it -> second -> updateThroughRect();
	}



}

void Map :: onSupportSprite( PlayerSprite* sprite)
{
	mapCollisionTree -> clear();
	for (int i = 0; i < collisionRectFs.size(); i++)
	{
		mapCollisionTree -> insert( collisionRectFs[i] );
  	}

	
#pragma region

	// player vs CollisionRects
	RectF r = sprite ->getBody();
	//r.height += 1;
	r.y += r.height;
	r.height = 1;
	std:: vector < CollisionRectF > returnList;
	mapCollisionTree -> getObjectlist (returnList, r);

	bool isSupported = false;
	for (int i = 0; i < returnList.size(); i++)
	{
		if( r.checkCollision( returnList[i].rect) )
		{
			sprite -> setSupportCollisionRect(returnList[i]);
			isSupported = true;
			break;
		}
	}
	// player vs object Map
	if(!isSupported)
	{
		for (std:: map < int ,ObjectSprite* > :: iterator objectIt = objectMap.begin() ; objectIt != objectMap.end(); objectIt++)
		{
			if(objectIt ->second ->isPlayerCollisionable())
			{
				CollisionRectF collisionRect = objectIt ->second ->getCollisionRect();
				if( r.checkCollision(collisionRect.rect)  )
				{
				
					if( objectIt ->second ->isAffectble() )
							sprite -> setSupportCollisionRect(collisionRect);
							isSupported = true;
							break;	
				}
			}	
		
		}
	}
	

	if(!isSupported)
	{
		sprite -> onUnsupported();
		sprite -> setSupportCollisionRect(CollisionRectF());
	}
	
#pragma endregion mapvsPlayer

#pragma region

 	for (std::map < int, EnermySprite* > ::iterator enermyIt = enermyMap.begin(); enermyIt != enermyMap.end(); enermyIt++)
	{
		RectF r = enermyIt ->second ->getBody();
		//r.height += 1;
		r.y += r.height;
		r.height = 1;
		std:: vector < CollisionRectF > returnLists;
		mapCollisionTree -> getObjectlist (returnLists, enermyIt -> second ->getBody());

		bool isSupported = false;

		for (int i = 0; i < returnLists.size(); i++)
		{

			if( r.checkCollision( returnLists[i].rect) )
			{
				enermyIt -> second -> setSupportCollisionRect(returnLists[i]);
				isSupported = true;
				break;
			}
			
		}

		if(!isSupported )
		{
			for (std:: map < int ,ObjectSprite* > :: iterator objectIt = objectMap.begin() ; objectIt != objectMap.end(); objectIt++)
			{
				if(objectIt ->second ->isEnemyCollisionable())
				{
					CollisionRectF collisionRect = objectIt ->second ->getCollisionRect();
					if( r.checkCollision(collisionRect.rect)  )
					{
				
						if( objectIt ->second ->isAffectble() )
								sprite -> setSupportCollisionRect(collisionRect);
								isSupported = true;
								break;	
					}
				}	
		
			}
		}
		


		if(!isSupported)
		{
			enermyIt -> second -> onUnsupported();
			enermyIt -> second -> setSupportCollisionRect(CollisionRectF());
		}


	}

#pragma endregion EnermyvsMap
	
}

void Map :: onUpdate(PlayerSprite* sprite, Camera* cam)
{
	cleanMap( cam , sprite);
	addEToMap( cam );

	// update enermy
	for (std::map < int, EnermySprite* > ::iterator it = enermyMap.begin(); it != enermyMap.end(); it++)
	{
		it ->second ->setPlayerX(sprite -> getCenterX());
		it ->second ->setPlayerY(sprite -> getCenterY());
		it ->second ->update();
	}


}
#pragma endregion publicFunction


