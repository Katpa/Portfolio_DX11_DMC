#include "Framework.h"
#include "MainGameScene.h"

MainGameScene::MainGameScene()
{
	dungeon = new Dungeon();
	dungeon->LoadDatas();
	dante = new Dante();

	dante->Pos() = dungeon->ReturnRoom(0)->Pos();

	dante->SetDungeon(dungeon);

	ENEMY->SetTarget(dante);

	FOR(dungeon->ReturnRoomSize())
	{
		PATH_FIND->SetNodes(i, dungeon->ReturnRoom(i)->ReturnNodes());
	}

	ENEMY->SetEmpusaAmount(dungeon->ReturnRoomSize());

	sky = new Skybox(L"Textures/Landscape/RedSky.dds");

	ParticleManager::Get()->Add("Blood", "TextData/Particles/Blood.fx", 10);
}

MainGameScene::~MainGameScene()
{
	delete dungeon;
	delete dante;
	UI->Delete();
	delete sky;

	ParticleManager::Delete();
}

void MainGameScene::Update()
{
	dante->Update();
	dungeon->Update();
	ENEMY->Update();

	UI->Update();
	ParticleManager::Get()->Update();
}

void MainGameScene::PreRender()
{
}

void MainGameScene::Render()
{
	sky->Render();
	dungeon->Render();
	ENEMY->Render();
	dante->Render();

	ParticleManager::Get()->Render();
}

void MainGameScene::PostRender()
{
	ENEMY->PostRender();
	UI->PostRender();
}

void MainGameScene::GUIRender()
{
	CAM->GUIRender();
	dante->GUIRender();
}
