#include "Framework.h"

DissolveModelAnimatorInstancing::DissolveModelAnimatorInstancing(string name)
    : ModelAnimator(name)
{
    SetShader(L"Model/DissolveModelAnimationInstancing.hlsl");

    instanceBuffer = new VertexBuffer(instanceDatas, sizeof(InstanceData), MAX_INSTANCE);
    frameInstancingBuffer = new FrameInstancingBuffer();
    dissolveInstancingBuffer = new DissolveInstancingBuffer();

    dissolveMap = Texture::Add(L"Textures/Effect/dissolve.jpg");
}

DissolveModelAnimatorInstancing::~DissolveModelAnimatorInstancing()
{
    delete instanceBuffer;
    delete frameInstancingBuffer;
    delete dissolveInstancingBuffer;
     
    for (Transform* transform : transforms)
        delete transform;
}

void DissolveModelAnimatorInstancing::Update()
{
    UpdateTransforms();
}

void DissolveModelAnimatorInstancing::Render()
{
    if (texture == nullptr)
        CreateTexture();

    instanceBuffer->Set(1);
    frameInstancingBuffer->SetVS(4);
    dissolveMap->PSSet(11);
    DC->VSSetShaderResources(0, 1, &srv);

    for (ModelMesh* mesh : meshes)
        mesh->RenderInstanced(drawCount);
}

void DissolveModelAnimatorInstancing::GUIRender()
{
    ImGui::Text("DrawCount : %d", drawCount);

    for (Transform* transform : transforms)
        transform->GUIRender();
}

Transform* DissolveModelAnimatorInstancing::Add()
{
    Transform* transform = new Transform();
    transform->SetTag(name + "_" + to_string(transforms.size()));
    transforms.push_back(transform);

    return transform;
}

void DissolveModelAnimatorInstancing::PlayClip(UINT instanceID, int clip, float scale, float takeTime)
{
    frameInstancingBuffer->Get().motions[instanceID].next.clip = clip;
    frameInstancingBuffer->Get().motions[instanceID].next.scale = scale;
    frameInstancingBuffer->Get().motions[instanceID].takeTime = takeTime;
    frameInstancingBuffer->Get().motions[instanceID].runningTime = 0.0f;
}

Matrix DissolveModelAnimatorInstancing::GetTransformByNode(UINT instanceID, int nodeIndex)
{
    if (texture == nullptr) return XMMatrixIdentity();

    Matrix curAnim;

    {//CurAnim
        Frame& curFrame = frameInstancingBuffer->Get().motions[instanceID].cur;

        Matrix cur = nodeTransforms[curFrame.clip].transform[curFrame.curFrame][nodeIndex];
        Matrix next = nodeTransforms[curFrame.clip].transform[curFrame.curFrame + 1][nodeIndex];

        curAnim = Lerp(cur, next, curFrame.time) * transforms[instanceID]->GetWorld();
    }

    {//NextAnim
        Frame& nextFrame = frameInstancingBuffer->Get().motions[instanceID].next;

        if (nextFrame.clip == -1)
            return curAnim;

        Matrix cur = nodeTransforms[nextFrame.clip].transform[nextFrame.curFrame][nodeIndex];
        Matrix next = nodeTransforms[nextFrame.clip].transform[nextFrame.curFrame + 1][nodeIndex];

        Matrix nextAnim = Lerp(cur, next, nextFrame.time) * transforms[instanceID]->GetWorld();

        float tweenTime = frameInstancingBuffer->Get().motions[instanceID].tweenTime;

        return Lerp(curAnim, nextAnim, tweenTime);
    }
}

void DissolveModelAnimatorInstancing::UpdateFrame(UINT instanceID, Motion& motion)
{
    {
        ModelClip* clip = clips[motion.cur.clip];

        motion.duration = clip->frameCount / clip->tickPerSecond;
        motion.runningTime += motion.cur.scale * DELTA;
        motion.cur.time += clip->tickPerSecond * motion.cur.scale * DELTA;

        if (motion.cur.time >= 1.0f)
        {
            motion.cur.curFrame = (motion.cur.curFrame + 1) % (clip->frameCount - 1);
            motion.cur.time -= 1.0f;
        }
    }

    {
        if (motion.next.clip < 0) return;

        ModelClip* clip = clips[motion.next.clip];

        motion.tweenTime += DELTA / motion.takeTime;

        if (motion.tweenTime >= 1.0f)
        {
            motion.cur = motion.next;
            motion.tweenTime = 0.0f;

            motion.next.clip = -1;
            motion.next.curFrame = 0;
            motion.next.time = 0.0f;
            return;
        }

        motion.next.time += clip->tickPerSecond * motion.next.scale * DELTA;

        if (motion.next.time >= 1.0f)
        {
            motion.next.curFrame = (motion.next.curFrame + 1) % (clip->frameCount - 1);
            motion.next.time -= 1.0f;
        }
    }
}

void DissolveModelAnimatorInstancing::UpdateTransforms()
{
    drawCount = 0;

    FOR(transforms.size())
    {
        if (transforms[i]->Active())
        {
            UpdateFrame(i, frameInstancingBuffer->Get().motions[i]);
            transforms[i]->UpdateWorld();
            instanceDatas[drawCount].world = XMMatrixTranspose(transforms[i]->GetWorld());
            instanceDatas[drawCount].index = i;
            instanceDatas[drawCount].dissolve = ENEMY->GetDissolve(i);
            drawCount++;
        }
    }

    instanceBuffer->Update(instanceDatas, drawCount);
}