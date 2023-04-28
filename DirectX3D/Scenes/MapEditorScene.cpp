#include "Framework.h"
#include "MapEditorScene.h"

MapEditorScene::MapEditorScene()
{
	dungeon = new Dungeon();
	curRoom = dungeon->ReturnRoom(1);
	curRoomPos = curRoom->Pos();
	SetRoom();
	SetModels();
}

MapEditorScene::~MapEditorScene()
{
	delete dungeon;

	for (auto statue : statues)
		delete statue;

	for (auto hesco : hescoes)
		delete hesco;
}

void MapEditorScene::Update()
{
	if (KEY_UP(VK_LBUTTON))
		Picking();

	if (dungeon->ReturnRoom(roomIndex) == curRoom) return;

	curRoom = dungeon->ReturnRoom(roomIndex);
}

void MapEditorScene::PreRender()
{
}

void MapEditorScene::Render()
{
	dungeon->Render(roomIndex);

	for (auto statue : statues)
		statue->Render();

	for (auto hesco : hescoes)
		hesco->Render();
}

void MapEditorScene::PostRender()
{
}

void MapEditorScene::GUIRender()
{
	int maxValue = dungeon->ReturnRoomSize() - 1;
	if (ImGui::DragInt("Room Index", (int*)&roomIndex, 1.0f, 0, maxValue))
		SetRoom();

	ImGui::Text("Obsjactile Type");
	if (ImGui::Button("NONE"))
		objIndex = 0;
	ImGui::SameLine();
	if (ImGui::Button("Statue"))
		objIndex = 1;
	ImGui::SameLine();
	if (ImGui::Button("Hesco"))
		objIndex = 2;

	if (ImGui::Button("Save cur room's Data"))
		Save();
	if (ImGui::Button("Load cur room's Data"))
		Load();
	if (ImGui::Button("Load All Datas"))
		dungeon->LoadDatas();
}

void MapEditorScene::SetRoom()
{
	if (curRoom->ReturnIndex() == roomIndex) return;
	curRoom->Pos() = curRoomPos;
	curRoom->UpdateTrans();
	curRoom = dungeon->ReturnRoom(roomIndex);
	curRoomPos = curRoom->Pos();
	curRoom->Pos() = {};
	curRoom->UpdateTrans();

	for (auto statue : statues)
		statue->SetActive(false);

	for (auto hesco : hescoes)
		hesco->SetActive(false);
}

void MapEditorScene::Picking()
{
	if (objIndex == 0) return;

	Ray ray = CAM->ScreenPointToRay(mousePos);
	vector<vector<JPSNode*>> nodes = curRoom->ReturnNodes();

	for (auto vec : nodes)
	{
		for (auto node : vec)
		{
			Collider* collider = node->ReturnCollider();
			if (!collider->IsRayCollision(ray, nullptr)) continue;
			
			if (objIndex == 1)
			{
				for (auto statue : statues)
				{
					if (statue->Active()) continue;

					statue->Pos().x = collider->GlobalPos().x;
					statue->Pos().z = collider->GlobalPos().z;
					statue->SetActive(true);
					statue->UpdateWorld();
					break;
				}
			}
			else
			{
				for (auto hesco : hescoes)
				{
					if (hesco->Active()) continue;

					hesco->Pos().x = collider->GlobalPos().x;
					hesco->Pos().z = collider->GlobalPos().z;
					hesco->SetActive(true);
					hesco->UpdateWorld();
					break;
				}
			}

			node->SetState(JPSNode::OBSTACLE);
		}
	}
}

void MapEditorScene::Save()
{
	vector<Transform*> sTrans, hTrans;

	for (auto statue : statues)
	{
		if (!statue->Active()) continue;

		Transform* trans = new Transform();
		trans->Pos() = statue->Pos();
		trans->Rot() = statue->Rot();
		trans->Scale() = statue->Scale();

		sTrans.push_back(trans);
	}

	for (auto hesco : hescoes)
	{
		if (!hesco->Active()) continue;

		Transform* trans = new Transform();
		trans->Pos() = hesco->Pos();
		trans->Rot() = hesco->Rot();
		trans->Scale() = hesco->Scale();

		hTrans.push_back(trans);
	}

	curRoom->SaveObstacle(sTrans, hTrans);

	for (auto t : sTrans)
		delete t;

	for (auto h : hTrans)
		delete h;

	curRoom->SaveNodes();
}

void MapEditorScene::Load()
{
	curRoom->LoadObstacle();
	curRoom->LoadNodes();
}

void MapEditorScene::LoadAll()
{
	UINT size = dungeon->ReturnRoomSize();

	FOR(size)
	{
		dungeon->ReturnRoom(i)->LoadObstacle();
	}
}

void MapEditorScene::SetModels()
{
	statues.resize(100);
	for (int i = 0; i < statues.size(); i++)
	{
		statues[i] = new Model("Statue");
		statues[i]->SetTag("Statue");
		statues[i]->Load();
		statues[i]->SetActive(false);
	}

	hescoes.resize(100);
	for (int i = 0; i < statues.size(); i++)
	{
		hescoes[i] = new Model("hesco");
		hescoes[i]->SetTag("Hesco");
		hescoes[i]->Load();
		hescoes[i]->SetActive(false);
	}
}