#include "Framework.h"

Hallway::Hallway(UINT index, vector<UINT> indices, const vector<vector<Vector3>>& dungeonTilePos, Vector3 hallwayPos, UINT worldWidth)
	:hallwayIndex(index), indices(indices), worldWidth(worldWidth)
{
	Pos() = hallwayPos;
	SetHallway(dungeonTilePos);

	tiles->SetParent(this);
	tiles->SetTextures(L"Textures/Dungeon/Hallway/Floor", L".jpg");
	walls->SetParent(this);
	walls->SetTextures(L"Textures/Dungeon/Hallway/Wall", L".jpg");
}

Hallway::~Hallway()
{
	delete tiles;
	delete walls;
	for (auto collider : wallColliders)
		delete collider;
}

void Hallway::Render()
{
	tiles->Render();
	walls->Render();

	for (auto collider : wallColliders)
		collider->Render();
}

BoxCollider* Hallway::IsWallCollision(Collider* playerBody)
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

void Hallway::SetHallway(const vector<vector<Vector3>>& dungeonTilePos)
{
	SetTiles(dungeonTilePos);
	SetWalls(dungeonTilePos);
}

void Hallway::SetTiles(const vector<vector<Vector3>>& dungeonTilePos)
{
	tiles = new InstancingTile();

	vector<Vector3> tilesPos;
	UINT worldWidth = dungeonTilePos[0].size();

	for (auto n : indices)
	{
		UINT h = n / worldWidth;
		UINT w = n % worldWidth;

		Vector3 tilePos = dungeonTilePos[h][w] - GlobalPos();

		tilesPos.push_back(tilePos);
	}

	tiles->SetFloorTranforms(tilesPos);
}

void Hallway::SetWalls(const vector<vector<Vector3>>& dungeonTilePos)
{
	////////////////
	//	UP = 0    //
	//	DOWN = 1  //
	//	LEFT = 2  //
	//	RIGHT = 3 //
	////////////////

	walls = new InstancingTile();

	vector<pair<Vector3, UINT>> wallTrans;

	UINT prevStart = indices[0] - (indices[1] - indices[0]);
	UINT nextEnd = indices[indices.size() - 1] + (indices[indices.size() - 1] - indices[indices.size() - 2]);

	for (int i = 0; i < 4; i++)
	{
		vector<UINT> equalLineIndices;
		vector<UINT> directionalIndices;
		bool isVertical = true;
		int next = 0;

		switch (i)
		{
		case 0:
			next = worldWidth;
			break;
		case 1:
			next = static_cast<int>(worldWidth) * -1;
			break;
		case 2:
			next = -1;
			isVertical = false;
			break;
		case 3:
			next = 1;
			isVertical = false;
			break;
		}

		for (int j = 0; j < indices.size(); j++)
		{
			UINT idx = indices[j] + next;

			if (idx == prevStart || idx == nextEnd) continue;

			bool isSkip = false;
			for (UINT n : indices)
			{
				if (idx != n) continue;
				isSkip = true;
				break;
			}

			if (isSkip) continue;

			if (equalLineIndices.size() > 1)
			{
				bool isTurn = false;

				if (!isVertical)
				{
					if (abs((int)equalLineIndices[equalLineIndices.size() - 1] - (int)idx) != worldWidth)
					{
						isTurn = true;
					}
				}
				else
				{
					if (abs((int)equalLineIndices[equalLineIndices.size() - 1] - (int)idx) != 1)
					{
						isTurn = true;
					}
				}

				if (isTurn)
				{
					for (auto index : equalLineIndices)
						directionalIndices.push_back(index);

					SetCollider(equalLineIndices, dungeonTilePos);
					isVertical = !isVertical;
				}
			}
			
			equalLineIndices.push_back(idx);
		}

		if (equalLineIndices.size() > 0)
		{
			for (auto index : equalLineIndices)
				directionalIndices.push_back(index);

			SetCollider(equalLineIndices, dungeonTilePos);
		}

		for (UINT index : directionalIndices)
		{
			UINT h = index / worldWidth;
			UINT w = index % worldWidth;

			Vector3 wallPos = dungeonTilePos[h][w];

			wallTrans.push_back(make_pair(wallPos, i));
		}
	}

	walls->SetWallTranforms(wallTrans);
}

void Hallway::SetCollider(vector<UINT>& equalLineIndices, const vector<vector<Vector3>>& dungeonTilePos)
{
	UINT indicesSize = equalLineIndices.size();

	if (indicesSize == 0) return;

	Vector3 colliderSize = { TILE_SIZE, WALL_HEIGHT, TILE_SIZE };

	float colliderPosY = WALL_HEIGHT * 0.5f;
	Vector3 colliderPos;

	if (indicesSize == 1)
	{
		UINT h = equalLineIndices[0] / worldWidth;
		UINT w = equalLineIndices[0] % worldWidth;

		colliderPos = dungeonTilePos[h][w];
	}
	else
	{
		UINT h1 = equalLineIndices[0] / worldWidth;
		UINT w1 = equalLineIndices[0] % worldWidth;
		Vector3 startPos = dungeonTilePos[h1][w1];

		UINT h2 = equalLineIndices[indicesSize - 1] / worldWidth;
		UINT w2 = equalLineIndices[indicesSize - 1] % worldWidth;
		Vector3 endPos = dungeonTilePos[h2][w2];

		colliderPos = (startPos + endPos) * 0.5f;

		if (startPos.x == endPos.x)
			colliderSize.z *= indicesSize;
		else
			colliderSize.x *= indicesSize;
	}

	colliderPos.y = colliderPosY;

	BoxCollider* collider = new BoxCollider(colliderSize);
	collider->Pos() = colliderPos;
	collider->UpdateWorld();
	
	wallColliders.push_back(collider);

	equalLineIndices.clear();
}
