#include "Framework.h"

Room::Room(UINT index, UINT width, UINT height, Vector3 roomPos, UINT lb, UINT rt, UINT worldWidth)
	:index(index), width(width), height(height), lb(lb), rt(rt), worldWidth(worldWidth)
{
	Pos() = roomPos;
	SetRoom();
	SetWallArea();

	tiles->SetParent(this);
	tiles->SetTextures(L"Textures/Dungeon/Room/Floor", L".jpg");
	walls->SetParent(this);
	walls->SetTextures(L"Textures/Dungeon/Room/Wall", L".jpg");

	statues = new ModelInstancing("Statue");
	hescoes = new ModelInstancing("Hesco");

	CreateJPSNodes();

	UpdateWorld();
	tiles->UpdateWorld();
	walls->UpdateWorld();
}

Room::~Room()
{
	for (vector<JPSNode*>& v : nodes)
	{
		for (auto n : v)
			delete n;

		v.clear();
	}
	nodes.clear();

	for (auto collider : wallColliders)
	{
		delete collider;
	}
	wallColliders.clear();

	delete statues;
	delete hescoes;
}

void Room::UpdateTrans()
{
	UpdateWorld();
	tiles->UpdateWorld();
	walls->UpdateWorld();
	for (Collider* collider : wallColliders)
		collider->UpdateWorld();

	for (auto vec : nodes)
		for (auto node : vec)
			node->UpdateWorld();

	statues->Update();
	hescoes->Update();
}

void Room::Render()
{
	tiles->Render();
	walls->Render();

	//---------Print Colliders---------
	//for (Collider* collider : wallColliders)
	//	collider->Render();

	//for (auto vec : nodes)
	//	for (auto node : vec)
	//		node->Render();
	//---------------------------------

	statues->Render();
	hescoes->Render();

	if (isGateOpen) return;

	for (Tile* gate : gates)
	{
		if (!gate) continue;
		gate->Render();
	}
}

void Room::CloseGate()
{
	ENEMY->SpawnEmpusa(index);
	isGateOpen = false;
}

void Room::SetGate(vector<vector<UINT>> hallwayIndices, const vector<vector<Vector3>>& dungeonTilePos)
{
	gates.resize(4);
	vector<UINT> gateIndices;
	FOR(hallwayIndices.size())
	{
		UINT lastIdx = hallwayIndices[i].size() - 1;
		SeperateWall(hallwayIndices[i][0], gateIndices);
		SeperateWall(hallwayIndices[i][lastIdx], gateIndices);
	}

	vector<pair<Vector3, UINT>> wallTrans;
	FOR(wallAreaIndices.size())
	{
		for (int j = 0; j < wallAreaIndices[i].size(); j++)
		{
			UINT h = wallAreaIndices[i][j] / worldWidth;
			UINT w = wallAreaIndices[i][j] % worldWidth;

			Vector3 wallPos = dungeonTilePos[h][w] - GlobalPos();

			wallTrans.push_back(make_pair(wallPos, wallsDir[i]));
		}
	}

	walls->SetWallTranforms(wallTrans);
	CreateWallCollider(dungeonTilePos);

	for (UINT idx : gateIndices)
	{
		if (idx < lb)
		{
			if (idx < lb - 1)
			{
				//하
				gates[DOWN] = new Tile();
				gates[DOWN]->SetParent(this);
				gates[DOWN]->Rot().x = XM_PI;
				gates[DOWN]->Rot().y = XM_PI;
				gates[DOWN]->Pos() = dungeonTilePos[CAST(UINT, idx / worldWidth)][CAST(UINT, idx % worldWidth)];
			}
			else
			{
				//좌
				gates[LEFT] = new Tile();
				gates[LEFT]->SetParent(this);
				gates[LEFT]->Rot().x = XM_PI;
				gates[LEFT]->Rot().y = XM_PIDIV2;
				gates[LEFT]->Pos() = dungeonTilePos[CAST(UINT, idx / worldWidth)][CAST(UINT, idx % worldWidth)];
			}
		}
		else
		{
			if (rt + 1 < idx)
			{
				//상
				gates[UP] = new Tile();
				gates[UP]->SetParent(this);
				gates[UP]->Rot().x = XM_PI;
				gates[UP]->Rot().y = 0;
				gates[UP]->Pos() = dungeonTilePos[CAST(UINT, idx / worldWidth)][CAST(UINT, idx % worldWidth)];
			}
			else
			{
				//우
				gates[RIGHT] = new Tile();
				gates[RIGHT]->SetParent(this);
				gates[RIGHT]->Rot().x = XM_PI;
				gates[RIGHT]->Rot().y = XM_PIDIV4 * 3.0f;
				gates[RIGHT]->Pos() = dungeonTilePos[CAST(UINT, idx / worldWidth)][CAST(UINT, idx % worldWidth)];
			}
		}
	}
}

void Room::LoadDatas()
{
	LoadObstacle();
	LoadNodes();
}

void Room::SaveObstacle(vector<Transform*> s, vector<Transform*> h)
{
	BinaryWriter* w = new BinaryWriter(L"MapData/RoomData/rd_" + to_wstring(index) + L".rdata");

	UINT size = s.size();

	w->UInt(size);

	FOR(size)
	{
		w->Float(s[i]->Pos().x);
		w->Float(s[i]->Pos().y);
		w->Float(s[i]->Pos().z);

		w->Float(s[i]->Rot().x);
		w->Float(s[i]->Rot().y);
		w->Float(s[i]->Rot().z);

		w->Float(s[i]->Scale().x);
		w->Float(s[i]->Scale().y);
		w->Float(s[i]->Scale().z);
	}

	size = h.size();

	w->UInt(size);

	FOR(size)
	{
		w->Float(h[i]->Pos().x);
		w->Float(h[i]->Pos().y);
		w->Float(h[i]->Pos().z);

		w->Float(h[i]->Rot().x);
		w->Float(h[i]->Rot().y);
		w->Float(h[i]->Rot().z);

		w->Float(h[i]->Scale().x);
		w->Float(h[i]->Scale().y);
		w->Float(h[i]->Scale().z);
	}
}

void Room::LoadObstacle()
{
	BinaryReader* r = new BinaryReader(L"MapData/RoomData/rd_" + to_wstring(index) + L".rdata");

	statues->Clear();
	hescoes->Clear();

	UINT size = r->UInt();

	if (size > 1000) return;

	FOR(size)
	{
		Transform* transform = statues->SetAdd();
		transform->Pos() = { r->Float(), r->Float(), r->Float() };
		transform->Rot() = { r->Float(), r->Float(), r->Float() };
		transform->Scale() = { r->Float(), r->Float(), r->Float() };
		transform->SetActive(true);
		transform->SetParent(this);
		transform->UpdateWorld();
	}

	size = r->UInt();

	FOR(size)
	{
		Transform* transform = hescoes->SetAdd();
		transform->Pos() = { r->Float(), r->Float(), r->Float() };
		transform->Rot() = { r->Float(), r->Float(), r->Float() };
		transform->Scale() = { r->Float(), r->Float(), r->Float() };
		transform->SetActive(true);
		transform->SetParent(this);
		transform->UpdateWorld();
	}

	statues->Update();
	hescoes->Update();

	CreateObstacleCollider();
}

void Room::SaveNodes()
{
	BinaryWriter* w = new BinaryWriter(L"MapData/NodeData/nd_" + to_wstring(index) + L".ndata");

	for (int i = 0; i < nodes.size(); i++)
	{
		for (int j = 0; j < nodes[i].size(); j++)
		{
			w->UInt(static_cast<UINT>(nodes[i][j]->GetState()));
		}
	}
}

void Room::LoadNodes()
{
	BinaryReader* r = new BinaryReader(L"MapData/NodeData/nd_" + to_wstring(index) + L".ndata");

	for (int i = 0; i < nodes.size(); i++)
	{
		for (int j = 0; j < nodes[i].size(); j++)
		{
			nodes[i][j]->SetState(static_cast<JPSNode::State>(r->UInt()));
		}
	}
}

BoxCollider* Room::IsWallCollision(Collider* playerBody)
{
	for (auto wall : wallColliders)
	{
		if (wall->IsCollision(playerBody))
		{
			return wall;
		}
	}

	return nullptr;
}

//-------------------------------------------------------------
// Code: void SetRoom()
// Desc: Setting room's initial values with data loaded
//-------------------------------------------------------------
void Room::SetRoom()
{
	tiles = new InstancingTile();
	tiles->SetFloorTranforms(width, height);
	walls = new InstancingTile();
}

//-------------------------------------------------------------
// Code: void SetWallArea(UINT worldWidth)
// Desc: Initialize the area that expected to collocate walls
//-------------------------------------------------------------
void Room::SetWallArea()
{
	wallAreaIndices.resize(4);

	FOR(width)
	{
		wallAreaIndices[DOWN].push_back(lb + i - worldWidth);
		wallAreaIndices[UP].push_back(lb + i + height * worldWidth);
	}

	FOR(height)
	{
		wallAreaIndices[LEFT].push_back((lb - 1) + i * worldWidth);
		wallAreaIndices[RIGHT].push_back((lb + width) + i * worldWidth);
	}

	wallsDir[UP] = UP;
	wallsDir[DOWN] = DOWN;
	wallsDir[LEFT] = LEFT;
	wallsDir[RIGHT] = RIGHT;
}

//-------------------------------------------------------------
// Code: void SeperateWall(UINT gateIndex, vector<UINT>& gateIndices)
// Desc: Seperate wall, if the gate is between wall and wall
//-------------------------------------------------------------
void Room::SeperateWall(UINT gateIndex, vector<UINT>& gateIndices)
{
	for (int q = 0; q < wallAreaIndices.size(); q++)
	{
		for (UINT i = 0; i < wallAreaIndices[q].size(); i++)
		{
			if (wallAreaIndices[q][i] != gateIndex) continue;

			gateIndices.push_back(wallAreaIndices[q][i]);

			if (i != 0 && i != wallAreaIndices[q].size() - 1)
			{
				// 분할할 필요가 있는 경우

				vector<UINT> tmp1, tmp2;

				for (int j = 0; j < i; j++)
				{
					tmp1.push_back(wallAreaIndices[q][j]);
				}

				for (int j = i + 1; j < wallAreaIndices[q].size(); j++)
				{
					tmp2.push_back(wallAreaIndices[q][j]);
				}

				wallAreaIndices[q].clear();

				wallAreaIndices[q] = tmp1;

				wallsDir[wallAreaIndices.size()] = wallsDir[q];
				wallAreaIndices.push_back(tmp2);
			}
			else
				wallAreaIndices[q].erase(wallAreaIndices[q].begin() + i);

			return;
		}
	}
}

//-------------------------------------------------------------
// Code: void CreateWallCollider()
// Desc: Create walls's colliders for collision checking of characters
//-------------------------------------------------------------
void Room::CreateWallCollider(const vector<vector<Vector3>>& dungeonTilePos)
{
	float colliderPosY = WALL_HEIGHT * 0.5f;

	FOR(wallAreaIndices.size())
	{
		Vector3 colliderSize;
		Vector3 colliderPos;

		UINT h = wallAreaIndices[i][0] / worldWidth;
		UINT w = wallAreaIndices[i][0] % worldWidth;

		if (wallAreaIndices[i].size() == 1)
		{
			colliderSize = { TILE_SIZE, WALL_HEIGHT, TILE_SIZE };
			colliderPos = { dungeonTilePos[h][w].x, colliderPosY, dungeonTilePos[h][w].z };
		}
		else
		{
			sort(wallAreaIndices[i].begin(), wallAreaIndices[i].end());
			UINT count = wallAreaIndices[i].size();

			UINT h2 = wallAreaIndices[i][wallAreaIndices[i].size() - 1] / worldWidth;
			UINT w2 = wallAreaIndices[i][wallAreaIndices[i].size() - 1] % worldWidth;

			colliderPos = (dungeonTilePos[h][w] + dungeonTilePos[h2][w2]) * 0.5f;
			colliderPos.y = colliderPosY;
			
			if (wallAreaIndices[i][1] - wallAreaIndices[i][0] > 1)
			{
				//Vertical
				colliderSize = { TILE_SIZE, WALL_HEIGHT, TILE_SIZE * count };
			}
			else
			{
				//horizen
				colliderSize = { TILE_SIZE * count, WALL_HEIGHT, TILE_SIZE };
			}
		}

		BoxCollider* collider = new BoxCollider(colliderSize);
		collider->Pos() = colliderPos - Pos();
		collider->SetParent(this);
		collider->UpdateWorld();

		wallColliders.push_back(collider);
	}
}

void Room::CreateObstacleCollider()
{
	vector<Transform*> trans = statues->ReturnTransform();
	for (auto tran : trans)
	{
		BoxCollider* collider = new BoxCollider(Vector3(TILE_SIZE, TILE_SIZE * 2.5f, TILE_SIZE));
		collider->Pos() = tran->GlobalPos();
		collider->Pos().y += TILE_SIZE * 0.75f;
		wallColliders.push_back(collider);
	}

	trans = hescoes->ReturnTransform();
	for (auto tran : trans)
	{
		BoxCollider* collider = new BoxCollider(Vector3(TILE_SIZE, TILE_SIZE, TILE_SIZE));
		collider->Pos() = tran->GlobalPos();
		collider->Pos().y += TILE_HALFSIZE;
		wallColliders.push_back(collider);
	}

	UpdateTrans();
}							   

void Room::CreateJPSNodes()
{
	vector<Transform*> transforms = tiles->ReturnTrans();
	UINT transIndex = 0;

	float colliderSize = TILE_HALFSIZE - TILE_HALFSIZE * 0.2f;

	for (int i = 0; i < height; i++)
	{
		vector<JPSNode*> vec;
		for (int j = 0; j < width; j++)
		{
			Vector2 pos = { transforms[transIndex]->Pos().x, transforms[transIndex]->GlobalPos().z };
			POINT nodeIdx = { i, j };
			vec.push_back(new JPSNode(pos, nodeIdx, colliderSize));
			vec[j]->SetParent(this);
			
			transIndex++;
		}

		nodes.push_back(vec);
	}
}