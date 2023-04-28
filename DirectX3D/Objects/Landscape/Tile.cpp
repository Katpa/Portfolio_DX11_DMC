#include "Framework.h"

Tile::Tile(UINT width, UINT height)
	: GameObject(L"Landscape/Tile.hlsl"), width(width), height(height)
{
    material->SetDiffuseMap(L"Textures/Landscape/Fieldstone_DM.tga");
    material->SetSpecularMap(L"Textures/Landscape/Fieldstone_SM.tga");
    material->SetNormalMap(L"Textures/Landscape/Fieldstone_NM.tga");

    ResetMesh();
}

Tile::~Tile()
{
    delete mesh;
}

void Tile::Render()
{
    GameObject::SetRender();
    mesh->Draw();
}

void Tile::SetRender()
{
    mesh->GetVertexBuffer()->Set();
    mesh->GetIndexBuffer()->Set();

    GameObject::SetRender();
}

void Tile::InstancedRender(UINT instanceCount)
{
    mesh->DrawInstanced(instanceCount);
}

void Tile::ResetMesh()
{
    if (mesh)
        delete mesh;

    mesh = new Mesh<VertexType>();
    MakeMesh();
    MakeNormal();
    MakeTangent();
    mesh->CreateMesh();
}

void Tile::MakeMesh()
{
    //Vertices
    vector<VertexType>& vertices = mesh->GetVertices();

    float left = width * -0.5f;
    float right = width * 0.5f;
    float top = height * 0.5f;
    float bottom = height * -0.5f;

    vertices.reserve(width * height);

    vertices.emplace_back(left, 0, top, 0, 0);
    vertices.emplace_back(right, 0, top, 1, 0);
    vertices.emplace_back(left, 0, bottom, 0, 1);
    vertices.emplace_back(right, 0, bottom, 1, 1);

    //Indices
    vector<UINT>& indices = mesh->GetIndices();
    indices = { 0, 1, 2, 2, 1, 3 };
}

void Tile::MakeNormal()
{
    vector<VertexType>& vertices = mesh->GetVertices();
    vector<UINT> indices = mesh->GetIndices();

    for (UINT i = 0; i < indices.size() / 3; i++)
    {
        UINT index0 = indices[i * 3 + 0];
        UINT index1 = indices[i * 3 + 1];
        UINT index2 = indices[i * 3 + 2];

        Vector3 v0 = vertices[index0].pos;
        Vector3 v1 = vertices[index1].pos;
        Vector3 v2 = vertices[index2].pos;

        Vector3 e0 = v1 - v0;
        Vector3 e1 = v2 - v0;

        Vector3 normal = Cross(e0, e1).GetNormalized();

        vertices[index0].normal = normal + vertices[index0].normal;
        vertices[index1].normal = normal + vertices[index1].normal;
        vertices[index2].normal = normal + vertices[index2].normal;
    }
}

void Tile::MakeTangent()
{
    vector<VertexType>& vertices = mesh->GetVertices();
    vector<UINT> indices = mesh->GetIndices();

    for (UINT i = 0; i < indices.size() / 3; i++)
    {
        UINT index0 = indices[i * 3 + 0];
        UINT index1 = indices[i * 3 + 1];
        UINT index2 = indices[i * 3 + 2];

        Vector3 p0 = vertices[index0].pos;
        Vector3 p1 = vertices[index1].pos;
        Vector3 p2 = vertices[index2].pos;

        Vector2 uv0 = vertices[index0].uv;
        Vector2 uv1 = vertices[index1].uv;
        Vector2 uv2 = vertices[index2].uv;

        Vector3 e0 = p1 - p0;
        Vector3 e1 = p2 - p0;

        float u1 = uv1.x - uv0.x;
        float v1 = uv1.y - uv0.y;
        float u2 = uv2.x - uv0.x;
        float v2 = uv2.y - uv0.y;

        float d = 1.0f / (u1 * v2 - v1 * u2);
        Vector3 tangent = d * (e0 * v2 - e1 * v1);

        vertices[index0].tangent += tangent;
        vertices[index1].tangent += tangent;
        vertices[index2].tangent += tangent;
    }
}
