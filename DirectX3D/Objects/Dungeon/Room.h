#pragma once

class Room : public Transform
{
private:
	enum Dir
	{
		UP = 0,
		DOWN = 1,
		LEFT = 2,
		RIGHT = 3
	};

public:
	Room(UINT index, UINT width, UINT height, Vector3 roomPos, UINT lb, UINT rt, UINT worldWidth);
	~Room();

	void UpdateTrans();
	void Render();

	void CloseGate();
	void OpenGate() { isGateOpen = true; }
	void SetGate(vector<vector<UINT>> hallwayIndices, const vector<vector<Vector3>>& dungeonTilePos);

	vector<vector<JPSNode*>> ReturnNodes() { return nodes; }

	UINT ReturnIndex() { return index; }

	void LoadDatas();

	void SaveObstacle(vector<Transform*> s, vector<Transform*> h);
	void LoadObstacle();

	void SaveNodes();
	void LoadNodes();

	BoxCollider* IsWallCollision(Collider* playerBody);

	bool IsEndBattle() { return isEndBattle; }
	UINT LB() { return lb; }
	UINT RT() { return rt; }

private:
	void SetRoom();

	void SetWallArea();
	void SeperateWall(UINT gateIndex, vector<UINT>& gateIndices);
	void CreateWallCollider(const vector<vector<Vector3>>& dungeonTilePos);
	void CreateObstacleCollider();

	void CreateJPSNodes();

private:
	UINT index;
	UINT lb, rt;
	UINT width;
	UINT height;
	UINT worldWidth;

	InstancingTile* tiles;
	InstancingTile* walls;
	map<UINT, Dir> wallsDir;
	vector<BoxCollider*> wallColliders;

	bool isEndBattle = false;
	bool isGateOpen = false;
	vector<Tile*> gates;

	vector<vector<UINT>> wallAreaIndices;

	vector<vector<JPSNode*>> nodes;

	ModelInstancing* statues;
	ModelInstancing* hescoes;
};