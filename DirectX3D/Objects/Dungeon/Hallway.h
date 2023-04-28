#pragma once

class Hallway : public Transform
{
public:
	Hallway(UINT index, vector<UINT> indices, const vector<vector<Vector3>>& dungeonTilePos, Vector3 hallwayPos, UINT worldWidth);
	~Hallway();

	void Render();
	
	void ON() { this->SetActive(true); }
	void OFF() { this->SetActive(false); }

	BoxCollider* IsWallCollision(Collider* playerBody);

	vector<UINT> ReturnIndices() { return indices; }

private:
	void SetHallway(const vector<vector<Vector3>>& dungeonTilePos);
	void SetTiles(const vector<vector<Vector3>>& dungeonTilePos);
	void SetWalls(const vector<vector<Vector3>>& dungeonTilePos);
	void SetCollider(vector<UINT>& equalLineIndices, const vector<vector<Vector3>>& dungeonTilePos);

private:
	UINT hallwayIndex;
	UINT worldWidth;
	vector<UINT> indices;

	InstancingTile* tiles;
	InstancingTile* walls;

	vector<vector<UINT>> wallAreaIndices;
	//vector<vector<UINT>> colliderIndices;

	vector<BoxCollider*> wallColliders;
};