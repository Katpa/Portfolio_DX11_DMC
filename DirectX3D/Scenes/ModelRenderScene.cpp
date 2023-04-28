#include "Framework.h"
#include "ModelRenderScene.h"

ModelRenderScene::ModelRenderScene()
{
    model = new Model("Rebellion");
    model->Load();

    collider = new CapsuleCollider();
    collider->SetParent(model);
}

ModelRenderScene::~ModelRenderScene()
{
    delete model;
}

void ModelRenderScene::Update()
{
    model->UpdateWorld();
    collider->UpdateWorld();
}

void ModelRenderScene::PreRender()
{
}

void ModelRenderScene::Render()
{
    model->Render();
    collider->Render();
}

void ModelRenderScene::PostRender()
{
}

void ModelRenderScene::GUIRender()
{
    model->GUIRender();
    collider->GUIRender();
}
