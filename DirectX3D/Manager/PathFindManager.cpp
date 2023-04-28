#include "Framework.h"

PathFindManager::PathFindManager()
{
	pool = new ThreadPool();

	jps.resize(JPS_SIZE);
	wip.reserve(JPS_SIZE);
	for (JPS*& obj : jps)
	{
		obj = new JPS();
		wip.push_back(false);
	}

	empusaWip.resize(MAX_UNIT_CNT);
	FOR(MAX_UNIT_CNT)
		empusaWip[i] = false;
}

PathFindManager::~PathFindManager()
{
	delete pool;

	for (auto obj : jps)
		delete obj;
	jps.clear();
}

void PathFindManager::AskPath(POINT start, POINT end, vector<Vector3>* pathes, UINT empusaIndex)
{
	if (empusaWip[empusaIndex]) return;

	ThreadBuffer buffer;
	buffer.firstParam = start;
	buffer.secondParam = end;
	buffer.pathes = pathes;
	buffer.index = empusaIndex;
	buffer.func = bind(&PathFindManager::GetPath, this, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4);
	pool->GetTask(buffer);
}

POINT PathFindManager::FindCloseNode(Vector3 pos)
{
	return jps[JPS_SIZE - 1]->FindCloseNode(Vector2(pos.x, pos.z));
}

void PathFindManager::GetPath(POINT startIndex, POINT endIndex, vector<Vector3>* pathes, UINT empusaIndex)
{
	vector<Vector3> path;

	empusaWip[empusaIndex] = true;

    FOR(wip.size())
    {
        if (wip[i]) continue;
		wip[i] = true;
    
		vector<Vector2> tmpPath;
        jps[i]->GetPath(startIndex, endIndex, tmpPath);

		path.reserve(tmpPath.size());
		for(int i = tmpPath.size() - 1; i > -1; i--)
		{
			path.push_back(Vector3(tmpPath[i].x, 0, tmpPath[i].y));
		}

		*pathes = path;

		wip[i] = false;
		break;
    }

	empusaWip[empusaIndex] = false;
}

void PathFindManager::SetNodes(UINT index, vector<vector<JPSNode*>> nodes)
{
	for (auto obj : jps)
		obj->AddRoom(index, nodes);
}

void PathFindManager::SetCurRoomIndex(UINT index)
{
	for (auto obj : jps)
		obj->SetRoomIndex(index);
}
