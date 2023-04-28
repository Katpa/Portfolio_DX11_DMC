#include "Framework.h"

Dungeon::Dungeon()
{
	LoadMapData();
	ENEMY->SetDungeon(this);

	CreateLight();
}

Dungeon::~Dungeon()
{
	for (auto room : rooms)
		delete room;
	rooms.clear();

	for (auto hallway : hallways)
		delete hallway;
	hallways.clear();
}

void Dungeon::Update()
{
}

void Dungeon::Render()
{
	for (auto room : rooms)
		room->Render();

	for (auto hallway : hallways)
		hallway->Render();
}

void Dungeon::Render(int index)
{
	rooms[index]->Render();
}

void Dungeon::CheckPlayer(Vector3 playerPos)
{
	//curActivatedRoom = nullptr;
	Room* tmpActivatedRoom = nullptr;
	curActivatedHallway = nullptr;

	for (auto room : rooms)
	{
		int h1 = room->LB() / mapSize.x;
		int w1 = room->LB() % mapSize.x;

		int h2 = room->RT() / mapSize.x;
		int w2 = room->RT() % mapSize.x;

		if (playerPos.x > dungeonTilePos[h1][w1].x - TILE_SIZE &&
			playerPos.z > dungeonTilePos[h1][w1].z - TILE_SIZE &&
			playerPos.x < dungeonTilePos[h2][w2].x + TILE_SIZE &&
			playerPos.z < dungeonTilePos[h2][w2].z + TILE_SIZE)
		{
			tmpActivatedRoom = room;
			PATH_FIND->SetCurRoomIndex(room->ReturnIndex());
			break;
		}
	}

	if (tmpActivatedRoom)
	{
		if (!curActivatedRoom || curActivatedRoom->ReturnIndex() != tmpActivatedRoom->ReturnIndex())
		{
			//게이트 닫고 몬스터 소환
			curActivatedRoom = tmpActivatedRoom;
			curActivatedRoom->CloseGate();
		}

		return;
	}

	for (auto hallway : hallways)
	{
		int firstIndex = hallway->ReturnIndices()[0];
		int secondIndex = hallway->ReturnIndices()[hallway->ReturnIndices().size() - 1];

		int h1 = firstIndex / mapSize.x;
		int w1 = firstIndex % mapSize.x;

		int h2 = secondIndex / mapSize.x;
		int w2 = secondIndex % mapSize.x;

		if (h1 > h2)
		{
			int tmp = h1;
			h1 = h2;
			h2 = tmp;
		}

		if (w1 > w2)
		{
			int tmp = w1;
			w1 = w2;
			w2 = tmp;
		}

		if (playerPos.x > dungeonTilePos[h1][w1].x - TILE_SIZE &&
			playerPos.z > dungeonTilePos[h1][w1].z - TILE_SIZE &&
			playerPos.x < dungeonTilePos[h2][w2].x + TILE_SIZE &&
			playerPos.z < dungeonTilePos[h2][w2].z + TILE_SIZE)
		{
			curActivatedHallway = hallway;
			break;
		}
	}
}

void Dungeon::LoadDatas()
{
	FOR(rooms.size())
		rooms[i]->LoadDatas();
}

BoxCollider* Dungeon::IsWallCollision(Collider* body)
{
	if (curActivatedHallway)
		return curActivatedHallway->IsWallCollision(body);
	else if (curActivatedRoom)
		return curActivatedRoom->IsWallCollision(body);

	return nullptr;
}
void Dungeon::LoadMapData()
{
	CreateRooms();
	CreateHallways();
	CreateWalls();
}

void Dungeon::CreateRooms()
{
	BinaryReader* r = new BinaryReader("MapData/RoomData.bsp");
	
	mapSize.x = r->UInt();
	mapSize.y = r->UInt();

	//===========================================================
	// Create grid of dungeon
	dungeonTilePos.resize(mapSize.y);
	for (int i = 0; i < dungeonTilePos.size(); i++)
	{
		dungeonTilePos[i].resize(mapSize.x);
		for (int j = 0; j < mapSize.x; j++)
		{
			dungeonTilePos[i][j] = {TILE_HALFSIZE + j * TILE_SIZE, 0, TILE_HALFSIZE + i * TILE_SIZE };
		}
	}
	//===========================================================

	UINT roomCnt = r->UInt();

	rooms.resize(roomCnt);

	FOR(roomCnt)
	{
		UINT roomWidth = r->UInt() - r->UInt() + 1;
		UINT roomHeight = r->UInt() - r->UInt() + 1;

		//던전 시작/끝 인덱스
		UINT startIdx = r->UInt();
		UINT endIdx = r->UInt();

		Vector3 roomPos = (dungeonTilePos[endIdx / mapSize.x][endIdx % mapSize.x] + dungeonTilePos[startIdx / mapSize.x][startIdx % mapSize.x]) / 2;

		rooms[i] = new Room(i, roomWidth, roomHeight, roomPos, startIdx, endIdx, mapSize.x);
	}

	delete r;
}

void Dungeon::CreateHallways()
{
	BinaryReader* r = new BinaryReader("MapData/HallwayData.bsp");

	UINT hallwayCnt = r->UInt();

	hallways.resize(hallwayCnt);

	FOR(hallwayCnt)
	{
		UINT idxCnt = r->UInt();
		vector<UINT> indices;
		for (int j = 0; j < idxCnt; j++)
		{
			indices.push_back(r->UInt());
		}
		Vector3 hallwayPos = (dungeonTilePos[indices[0] / mapSize.x][indices[0] % mapSize.x] +
							  dungeonTilePos[indices[indices.size() - 1] / mapSize.x][indices[indices.size() - 1] % mapSize.x]) * 0.5f;

		hallways[i] = new Hallway(i, indices, dungeonTilePos, hallwayPos, mapSize.x);
	}

	delete r;
}

void Dungeon::CreateWalls()
{
	vector<vector<UINT>> hallwayIndices;
	FOR(hallways.size())
	{
		hallwayIndices.push_back(hallways[i]->ReturnIndices());
	}

	FOR(rooms.size())
	{
		rooms[i]->SetGate(hallwayIndices, dungeonTilePos);
	}
}

void Dungeon::CreateLight()
{
	LightBuffer::Light* light = Environment::Get()->AddLight();

	light->color.x = 141.0f / 255.0f;
	light->color.y = 156.0f / 255.0f;
	light->color.z = 162.0f / 255.0f;

	light->direction.x = -0.235;
	light->direction.y = -0.058;
	light->direction.z = -0.132;
}
