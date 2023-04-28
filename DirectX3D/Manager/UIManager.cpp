#include "Framework.h"

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
	delete player;
}

void UIManager::Update()
{
	player->Update();
}

void UIManager::PostRender()
{
	player->hpBar->Render();
}
