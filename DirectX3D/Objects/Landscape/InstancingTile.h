#pragma once

class InstancingTile : public Transform
{
private:
	const UINT MAX_INSTANCING = 1024;

private:
	struct InstanceData
	{
		Matrix transform = XMMatrixIdentity();
	};

public:
	InstancingTile();
	~InstancingTile();

	void UpdateWorld();
	void Render();

	void SetTextures(wstring file, wstring format);

	void SetFloorTranforms(UINT width, UINT height);
	void SetFloorTranforms(vector<Vector3> tilePos);
	void SetWallTranforms(vector<pair<Vector3, UINT>> wallTrans);

	void GUIRender() { tile->GetMaterial()->GUIRender(); Transform::GUIRender(); }
	vector<Transform*> ReturnTrans() { return transforms; }
private:
	

private:
	Tile* tile;

	UINT existCount;
	UINT drawCount;

	vector<InstanceData> instances;
	vector<Transform*> transforms;
	VertexBuffer* instanceBuffer;
};