#pragma once

class JPS
{
public:
	JPS();
	JPS(class Dungeon* map);
	~JPS();

	void Update();
	void Render();

	POINT FindCloseNode(Vector2 pos);
	void GetPath(IN POINT start, IN POINT end, OUT vector<Vector2>& path);

	void AddRoom(UINT roomIndex, vector<vector<JPSNode*>> nodeSet) { rooms[roomIndex] = nodeSet; }
	void SetRoomIndex(UINT index) { roomIndex = index; }

private:
	void Reset();

	float GetDistance(POINT start, POINT end);

	void DefaultSearch();

	void StraightSearch(JPSNode* node);
	void DiagonalSearch(JPSNode* node);
	
	bool VerticalSearch(JPSNode* curNode, POINT idx, Vector2 dir);
	bool HorizentalSearch(JPSNode* curNode, POINT idx, Vector2 dir);
	bool SubSearch(JPSNode* node);

	void AddNeighbor(JPSNode* parent, POINT index, Vector2 dir);

	bool IsInWidthRange(POINT idx) { return idx.y < mapSize.x && idx.y > -1; }
	bool IsInHeightRange(POINT idx) { return idx.x < mapSize.y && idx.x > -1; }

	JPSNode*& NODE(POINT idx) { return curRoom[idx.x][idx.y]; }
	JPSNode*& NODE(UINT x, UINT y) { return curRoom[x][y]; }

	bool IsPossible(JPSNode* node);
	bool IsEndNode(POINT idx);

private:
	map<UINT, vector<vector<JPSNode*>>> rooms;
	vector<vector<JPSNode*>> curRoom;

	UINT roomIndex;

	JPSNode* target = nullptr;
	bool isEnd = false;
	JPSHeap* openList;
	POINT mapSize;

	Dungeon* dungeon;

	const vector<Vector2> allDir = { {1, 0}, {0, 1}, {1, 1}, {-1, 1}, {-1, 0}, {0, -1}, {-1, -1}, {1, -1} };
};