#pragma once

//-----Note-----
// �׷� ���� �÷��̾��� ��ġ�� ���� ��� �ڵ鸵 �����ؾ���
//

class Dungeon
{
private:
	struct TileData
	{
		Vector3 pos;
	};

public:
	Dungeon();
	~Dungeon();

	void Update();
	void Render();
	void Render(int index);

	void CheckPlayer(Vector3 playerPos);
	void SetJPSNodes(IN UINT roomIndex, OUT vector<vector<JPSNode*>>& nodes) { nodes = rooms[roomIndex]->ReturnNodes(); }
	
	void LoadDatas();

	BoxCollider* IsWallCollision(Collider* body);
	
	Room* ReturnRoom(int index) { return rooms[index]; }
	int ReturnRoomSize() { return rooms.size(); }
	UINT ReturnCurRoomIndex() { return curActivatedRoom ? curActivatedRoom->ReturnIndex() : -1; }

private:
	void LoadMapData();
	void CreateRooms();
	void CreateHallways();
	void CreateWalls();

	void CreateLight();

private:
	POINT mapSize;
	vector<TileData> mapData;
	vector<Room*> rooms;
	vector<Hallway*> hallways;

	vector<vector<Vector3>> dungeonTilePos;

	Room* curActivatedRoom = nullptr;
	Hallway* curActivatedHallway = nullptr;
};