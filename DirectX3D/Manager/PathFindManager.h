#pragma once

class PathFindManager : public Singleton<PathFindManager>
{
private:
	const UINT JPS_SIZE = 10;

public:
	PathFindManager();
	~PathFindManager();

	void AskPath(POINT start, POINT end, vector<Vector3>* pathes, UINT empusaIndex);
	POINT FindCloseNode(Vector3 pos);

	void GetPath(POINT startIndex, POINT endIndex, vector<Vector3>* pathes, UINT empusaIndex);
	void SetNodes(UINT index, vector<vector<JPSNode*>> nodes);
	void SetCurRoomIndex(UINT index);

	void SetPathReady(UINT index) { empusaWip[index] = false; }

private:
	ThreadPool* pool;
	vector<JPS*> jps;
	vector<bool> wip;
	vector<bool> empusaWip;
};