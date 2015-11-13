
#include "Game.h"
#include <vector>
#include "iostream"

Game:: Game()
{
	SceneManager::getInstance() ->createScene(new PlayScene(1));
}

//void Game :: go()
//{
//	SceneManager::getInstance() ->getCurrentScene()->go();
//}

void Game :: update()
{
	SceneManager::getInstance() ->getCurrentScene() ->onUpdate();
}

void Game :: render()
{
	SceneManager::getInstance() ->getCurrentScene() ->render();
}

