#include "Framework.h"

InstancingTile::InstancingTile()
	:drawCount(0)
{
	tile = new Tile();
	tile->GetMaterial()->SetShader(L"Landscape/TileInstancing.hlsl");
	tile->SetParent(this);

	instances.resize(MAX_INSTANCING);

	transforms.resize(MAX_INSTANCING);
	FOR(transforms.size())
	{
		transforms[i] = new Transform();
		transforms[i]->SetParent(this);
		transforms[i]->SetActive(false);
	}

	instanceBuffer = new VertexBuffer(instances.data(), sizeof(InstanceData), MAX_INSTANCING);
}

InstancingTile::~InstancingTile()
{
	delete instanceBuffer;
	delete tile;

	for (auto trans : transforms)
		delete trans;
	
	transforms.clear();
}

void InstancingTile::UpdateWorld()
{
	Transform::UpdateWorld();
	tile->UpdateWorld();
	FOR(transforms.size())
	{
		if (!transforms[i]->Active()) continue;
		if (!CAM->ContainPoint(transforms[i]->GlobalPos())) continue;

		transforms[i]->UpdateWorld();
		instances[i].transform = XMMatrixTranspose(transforms[i]->GetWorld());
	}

	existCount = drawCount;
	instanceBuffer->Update(instances.data(), drawCount);
}

void InstancingTile::Render()
{
	instanceBuffer->Set(1);
	tile->SetRender();
	Environment::Get()->GetLight(0);
	DC->DrawIndexedInstanced(6, drawCount, 0, 0, 0);
}

void InstancingTile::SetTextures(wstring file, wstring format)
{
	wstring dm = file + L"_DM" + format;
	wstring sm = file + L"_SM" + format;
	wstring nm = file + L"_NM" + format;

	tile->GetMaterial()->SetDiffuseMap(dm);
	tile->GetMaterial()->SetSpecularMap(sm);
	tile->GetMaterial()->SetNormalMap(nm);
	
	tile->ResetMesh();
}

void InstancingTile::SetFloorTranforms(UINT width, UINT height)
{
	drawCount = 0;
	this->UpdateWorld();

	float x = static_cast<float>(width - 1) * TILE_HALFSIZE;
	float y = static_cast<float>(height - 1) * TILE_HALFSIZE;

	Vector3 transPos = { -x, 0, -y };

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			transforms[drawCount]->Pos() = transPos;
			transforms[drawCount]->SetActive(true);
			transforms[drawCount]->UpdateWorld();

			instances[drawCount].transform = XMMatrixTranspose(transforms[drawCount]->GetWorld());
			drawCount++;

			transPos.x += TILE_SIZE;
		}

		transPos = { -x, 0, -y + (i + 1) * TILE_SIZE };
	}
	instanceBuffer->Update(instances.data(), drawCount);
}

void InstancingTile::SetFloorTranforms(vector<Vector3> tilePos)
{
	drawCount = 0;
	this->UpdateWorld();

	for (auto pos : tilePos)
	{
		transforms[drawCount]->Pos() = pos;
		transforms[drawCount]->SetActive(true);
		transforms[drawCount]->UpdateWorld();

		instances[drawCount].transform = XMMatrixTranspose(transforms[drawCount]->GetWorld());
		drawCount++;
	}

	instanceBuffer->Update(instances.data(), drawCount);
}

void InstancingTile::SetWallTranforms(vector<pair<Vector3, UINT>> wallTrans)
{
	////////////////
	//	UP = 0    //
	//	DOWN = 1  //
	//	LEFT = 2  //
	//	RIGHT = 3 //
	////////////////

	this->UpdateWorld();
	drawCount = 0;

	FOR(wallTrans.size())
	{
		UINT dir = wallTrans[i].second;

		for (int j = i * WALL_TILE_CNT; j < (i + 1) * WALL_TILE_CNT; j++)
		{
			transforms[j]->Pos() = wallTrans[i].first;
			transforms[j]->Pos().y += TILE_HALFSIZE + TILE_SIZE * (j % WALL_TILE_CNT);
			transforms[j]->Rot().x = XM_PIDIV2;
			switch (dir)
			{
			case 0:
				transforms[j]->Rot().y = XM_PI;
				transforms[j]->Pos().z -= TILE_HALFSIZE;
				break;
			case 1:
				transforms[j]->Rot().y = 0;
				transforms[j]->Pos().z += TILE_HALFSIZE;
				break;
			case 2:
				transforms[j]->Rot().y = XM_PIDIV2;
				transforms[j]->Pos().x += TILE_HALFSIZE;
				break;
			case 3:
				transforms[j]->Rot().y = XM_PI + XM_PIDIV2;
				transforms[j]->Pos().x -= TILE_HALFSIZE;
				break;
			}

			transforms[j]->SetActive(true);
			transforms[j]->UpdateWorld();
			drawCount++;
			instances[j].transform = XMMatrixTranspose(transforms[j]->GetWorld());
		}
	}

	instanceBuffer->Update(instances.data(), drawCount);
}