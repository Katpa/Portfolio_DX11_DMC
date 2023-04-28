#include "Framework.h"

JPS::JPS()
{
    openList = new JPSHeap();
}

JPS::JPS(Dungeon* map)
    :dungeon(map)
{
    // �뿡�� ������ �� JPSNode���� �����ϰ� JPS���� '����'�� ���� �޾ƾ� ��
    // �ֳ��ϸ� �����带 ����ϰ� �Ǹ� ��� ������ ���� �� �ֱ� ������
    //map->GetNodes(nodes, mapSize);
    //SetEdge(map->GetWidth());

    openList = new JPSHeap();
}

JPS::~JPS()
{
    for (auto p : rooms)
    {
        for (vector<JPSNode*> vec : p.second)
        {
            for (JPSNode* node : vec)
            {
                delete node;
            }

            vec.clear();
        }
    }
    rooms.clear();
    
    delete openList;
}

void JPS::Update()
{
}

void JPS::Render()
{
    //for (vector<JPSNode*> vec : nodes)
    //    for (JPSNode* node : vec)
    //        node->Render();
}

//-------------------------------------------------------------
// Code: int FindCloseNode(Vector2 pos)
// Desc: ���� ��ǥ�� ���� ���� ����� ����� �ε����� ��ȯ�ϴ� �Լ�
//-------------------------------------------------------------
POINT JPS::FindCloseNode(Vector2 pos)
{
    float minDist = FLT_MAX;

    POINT index = {};

    for (int i = 0; i < rooms[roomIndex].size(); i++)
    {
        for (int j = 0; j < rooms[roomIndex][i].size(); j++)
        {
            if (rooms[roomIndex][i][j]->state == JPSNode::OBSTACLE) continue;

            float distance = Distance(pos, rooms[roomIndex][i][j]->Vec2Pos());

            if (minDist > distance)
            {
                minDist = distance;
                index.x = i;
                index.y = j;
            }
        }
    }

    return index;
}

//-------------------------------------------------------------
// Code: void GetPath(int start, int end, vector<Vector2>& path)
// Desc: �������� �������� ������ ��� ������ path ���Ϳ� �����ϴ� �Լ�
//-------------------------------------------------------------
void JPS::GetPath(IN POINT start, IN POINT end, OUT vector<Vector2>& path)
{
    Reset();
    path.clear();
    openList->Clear();

    float G = 0;
    float H = GetDistance(start, end);

    curRoom[start.x][start.y]->g = G;
    curRoom[start.x][start.y]->h = H;
    curRoom[start.x][start.y]->f = G + H;

    target = curRoom[end.x][end.y];

    // 1. ���� ��带 ���� ����Ʈ�� �ְ� 8���� �ʱ� Ž���� �ǽ��Ѵ�.
    openList->Insert(curRoom[start.x][start.y]);
    DefaultSearch();

    // ��� ���� ������ 2~5�� �ݺ�
    // ���� ����Ʈ�� ����ִٸ� �� �̻� ��ΰ� ������ �ǹ���
    while (!openList->Empty())
    {
        // 2. ���� ����Ʈ���� F���� ���� ���� ��带 ���� ���� �����ϰ� ���� ����Ʈ���� ���� �� Ŭ���� ���·� �����
        JPSNode* curNode = openList->GetMinCostNode();
        POINT curIndex = curNode->index;

        // 3. ��忡 ��ϵ� ���⿡ ���� ����Ž���� �缱Ž�� �� �� �ϳ��� ����
        if(curNode->dir.x + curNode->dir.y == 1 || curNode->dir.x + curNode->dir.y == -1)
            StraightSearch(curNode);
        else
            DiagonalSearch(curNode);

        // 4. ������ ����
        if (isEnd) break;

        curNode->state = JPSNode::CLOSE;
    }

    JPSNode* tmpNode = target;
    vector<JPSNode*> tmpVec;

    while (tmpNode)
    {
        tmpNode->state = JPSNode::PATH;
        tmpVec.push_back(tmpNode);
        tmpNode = tmpNode->parent;

        if (tmpNode == nullptr || tmpNode == tmpNode->parent)
            break;
    }

    for (int i = 0; i < tmpVec.size(); i++)
        path.push_back(tmpVec[i]->Vec2Pos());
}

//-------------------------------------------------------------
// Code: void Reset()
// Desc: Ž�� �ʱ⿡ �� ������ ���¸� �ʱ�ȭ�ϴ� �Լ�
//-------------------------------------------------------------
void JPS::Reset()
{
    curRoom = rooms[roomIndex];
    mapSize = { CAST(long, curRoom[0].size()), CAST(long, curRoom.size())};
    isEnd = false;

    for (vector<JPSNode*> vec : curRoom)
    {
        for (JPSNode* node : vec)
        {
            if (node->state != JPSNode::OBSTACLE)
                node->SetInit();
        }
    }
}

float JPS::GetDistance(POINT start, POINT end)
{
    Vector2 startPos = curRoom[start.x][start.y]->Vec2Pos();
    Vector2 endPos = curRoom[end.x][end.y]->Vec2Pos();

    Vector2 tmp = endPos - startPos;

    return tmp.Length();
}

void JPS::DefaultSearch()
{
    JPSNode* node = openList->GetMinCostNode();

    for(Vector2 dir : allDir)
    {
        node->dir = dir;

        if (abs(dir.x + dir.y) == 1)
            StraightSearch(node);
        else
            DiagonalSearch(node);
    }
}

//-------------------------------------------------------------
// Code: void StraightSearch(JPSNode* node)
// Desc: ���� ���� Ž���� �ϴ� �Լ�
//-------------------------------------------------------------
void JPS::StraightSearch(JPSNode* node)
{
    Vector2 dir = node->dir;
    POINT idx = node->index;

    if (dir.x)
    {
        // Horizental Search
        HorizentalSearch(node, idx, dir);
    }
    else
    {
        // Vertical Search
        VerticalSearch(node, idx, dir);
    }
}

//-------------------------------------------------------------
// Code: void DiagonalSearch(JPSNode* node)
// Desc: �缱 Ž���� �ϴ� �Լ�
//-------------------------------------------------------------
void JPS::DiagonalSearch(JPSNode* node)
{
    Vector2 dir = node->dir;
    POINT idx = node->index;

    bool isSearched = false;
    bool horizental = false;
    bool vertical = false;

    if (!(idx.x - dir.y >= 0 && idx.x - dir.y < mapSize.y)) return;
    if (!(idx.y - dir.x >= 0 && idx.y - dir.x < mapSize.y)) return;

    while (true)
    {
        if (!IsInHeightRange(idx)) break;
        if (!IsInWidthRange(idx)) break;
        if (NODE(idx)->state == JPSNode::OBSTACLE) break;
        if (IsEndNode(idx))
        {
            NODE(idx)->SetNode(node);
            break;
        }

        // Horizental
        if (idx.y + dir.x > -1 && idx.y + dir.x < mapSize.x && idx.x - dir.y > -1 && idx.x - dir.y < mapSize.y)
        {
            if (NODE(idx.x - dir.y, idx.y)->state == JPSNode::OBSTACLE &&
                NODE(idx.x - dir.y, idx.y + dir.x)->state == JPSNode::NONE)
            {
                horizental = true;
            }
        }

        // Vertical
        if (idx.x + dir.y > -1 && idx.x + dir.y < mapSize.y && idx.y - dir.x > -1 && idx.y - dir.x < mapSize.x)
        {
            if (NODE(idx.x, idx.y - dir.x)->state == JPSNode::OBSTACLE &&
                NODE(idx.x + dir.y, idx.y - dir.x)->state == JPSNode::NONE)
            {
                vertical = true;
            }
        }

        if (vertical == !horizental)
        {
            POINT extraIdx;
            Vector2 extraDir;
            if (horizental)
            {
                extraIdx.x = idx.x - dir.y;
                extraIdx.y = idx.y + dir.x;
                extraDir = { dir.x, -dir.y };
            }
            else
            {
                extraIdx.x = idx.x + dir.y;
                extraIdx.y = idx.y - dir.x;
                extraDir = { -dir.x, dir.y };
            }
            AddNeighbor(NODE(idx), extraIdx, extraDir);
            isSearched = true;
        }

        if (VerticalSearch(NODE(idx), idx, Vector2(0, dir.y)))
        {
            NODE(idx)->SetNode(node);
            isSearched = true;
        }

        if (HorizentalSearch(NODE(idx), idx, Vector2(dir.x, 0)))
        {
            NODE(idx)->SetNode(node);
            isSearched = true;
        }

        idx.x += dir.y;
        idx.y += dir.x;

        if (isSearched)
        {
            if (!IsInHeightRange(idx)) break;
            if (!IsInWidthRange(idx)) break;

            AddNeighbor(node, idx, dir);
            break;
        }
    }
}

//-------------------------------------------------------------
// Code: void SubSearch(JPSNode* curNode)
// Desc: �缱 Ž�� �� �缱 ��θ� ���� ���� Ž���ϴ� �Լ�
//-------------------------------------------------------------
bool JPS::SubSearch(JPSNode* node)
{
    
    return false;
}

//-------------------------------------------------------------
// Code: bool VerticalSearch(JPSNode* curNode, Vector2 dir, vector<JPSNode*>& forcedNeighbors)
// Desc: ���� Ž���ϴ� �Լ�
//-------------------------------------------------------------
bool JPS::VerticalSearch(JPSNode* curNode, POINT idx, Vector2 dir)
{
    bool isSearched = false;

    while (true)
    {
        idx.x += dir.y;

        if (!IsInHeightRange(idx)) break;
        if (NODE(idx)->state == JPSNode::OBSTACLE) break;
        if (IsEndNode(idx))
        {
            NODE(idx)->SetNode(curNode);
            return true;
        }

        // Right
        if (idx.y < mapSize.x - 1 && idx.x < mapSize.y - 1 && idx.x > 0)
        {
            if (NODE(idx.x, idx.y + 1)->state == JPSNode::OBSTACLE &&
                NODE(idx.x + dir.y, idx.y + 1)->state == JPSNode::NONE)
            {
                POINT extraIdx = { idx.x + dir.y, idx.y + 1 };
                Vector2 extraDir(1, dir.y);
                AddNeighbor(NODE(idx), extraIdx, extraDir);
                isSearched = true;
            }
        }

        // Left
        if (idx.y > 0 && idx.x < mapSize.y - 1 && idx.x > 0)
        {
            if (NODE(idx.x, idx.y - 1)->state == JPSNode::OBSTACLE &&
                NODE(idx.x + dir.y, idx.y - 1)->state == JPSNode::NONE)
            {
                POINT extraIdx = { idx.x + dir.y, idx.y - 1 };
                Vector2 extraDir(-1, dir.y);
                AddNeighbor(NODE(idx), extraIdx, extraDir);
                isSearched = true;
            }
        }

        if (isSearched)
        {
            AddNeighbor(curNode, idx, dir);
            return true;
        }
    }

    return false;
}

//-------------------------------------------------------------
// Code: bool HorizentalSearch(JPSNode* curNode, Vector2 dir, vector<JPSNode*>& forcedNeighbors)
// Desc: ���� Ž���ϴ� �Լ�
//-------------------------------------------------------------
bool JPS::HorizentalSearch(JPSNode* curNode, POINT idx, Vector2 dir)
{
    bool isSearched = false;

    while (true)
    {
        idx.y += dir.x;

        if (!IsInWidthRange(idx)) return false;
        if (NODE(idx)->state == JPSNode::OBSTACLE) return false;
        if (IsEndNode(idx))
        {
            NODE(idx)->SetNode(curNode);
            return true;
        }

        // upper
        if (idx.x < mapSize.y - 1 && idx.y < mapSize.x - 1 && idx.y > 0)
        {
            if (NODE(idx.x + 1, idx.y)->state == JPSNode::OBSTACLE &&
                NODE(idx.x + 1, idx.y + dir.x)->state == JPSNode::NONE)
            {
                POINT extraIdx = { idx.x + 1, idx.y + dir.x };
                Vector2 extraDir(dir.x, 1);
                AddNeighbor(NODE(idx), extraIdx, extraDir);

                isSearched = true;
            }
        }

        // lower
        if (idx.x > 0 && idx.y < mapSize.x - 1 && idx.y > 0)
        {
            if (NODE(idx.x - 1, idx.y)->state == JPSNode::OBSTACLE &&
                NODE(idx.x - 1, idx.y + dir.x)->state == JPSNode::NONE)
            {
                POINT extraIdx = { idx.x - 1, idx.y + dir.x };
                Vector2 extraDir(dir.x, -1);
                AddNeighbor(NODE(idx), extraIdx, extraDir);

                isSearched = true;
            }
        }

        if (isSearched)
        {
            AddNeighbor(curNode, idx, dir);
            return true;
        }
    }

    return false;
}

//-------------------------------------------------------------
// Code: void AddNeighbor(JPSNode* parent, POINT index, Vector2 dir)
// Desc: �̿� ��带 openList�� �߰��ϰ� ���³��� �����ϴ� �Լ�
//-------------------------------------------------------------
void JPS::AddNeighbor(JPSNode* parent, POINT index, Vector2 dir)
{
    JPSNode* neighbor = curRoom[index.x][index.y];
    neighbor->SetNode(parent, dir);
    openList->Insert(NODE(index));
}

//-------------------------------------------------------------
// Code: bool IsPossible(JPSNode* node)
// Desc: ���� Ž�� ���⿡ Ž�� �ߴ� ���� �˻� �Լ�
//-------------------------------------------------------------
bool JPS::IsPossible(JPSNode* node)
{
    
    return true;
}

//-------------------------------------------------------------
// Code: bool IsEndNode(POINT idx)
// Desc: �Է¹��� �ε����� ������ ���� ���� �ε������� Ȯ���ϴ� �Լ�
//-------------------------------------------------------------
bool JPS::IsEndNode(POINT idx)
{
    if (NODE(idx) == target)
    {
        isEnd = true;
        return true;
    }

    return false;
}
