#pragma once

class Tile : public GameObject
{
private:
	typedef VertexUVNormalTangent VertexType;

public:
	Tile(UINT width = TILE_SIZE, UINT height = TILE_SIZE);
	~Tile();

    void Render() override;
    void SetRender();
    void InstancedRender(UINT instanceCount);

    void ResetMesh();

private:
    void MakeMesh();
    void MakeNormal();
    void MakeTangent();

private:
    UINT width, height;

    Mesh<VertexType>* mesh;
};