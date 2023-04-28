#pragma once

class DissolveModelAnimatorInstancing : public ModelAnimator
{
public:
    struct Motion
    {
        float takeTime = 0.2f;
        float tweenTime = 0.0f;
        float runningTime = 0.0f;
        float duration = 0.0f;

        Frame cur, next;

        Motion()
        {
            next.clip = -1;
        }
    };

private:
    class FrameInstancingBuffer : public ConstBuffer
    {
    public:
        struct Data
        {
            Motion motions[MAX_INSTANCE];
        };

    public:
        FrameInstancingBuffer() : ConstBuffer(&data, sizeof(Data))
        {
        }

        Data& Get() { return data; }

    private:
        Data data;
    };

    class DissolveInstancingBuffer : public ConstBuffer
    {
    public:
        struct Data
        {
            float dissolves[MAX_INSTANCE];
        };

    public:
        DissolveInstancingBuffer() : ConstBuffer(&data, sizeof(Data))
        {
            FOR(MAX_INSTANCE)
            {
                Get().dissolves[i] = 1.0f;
            }
        }

        Data& Get() { return data; }

    private:
        Data data;
    };

public:
    DissolveModelAnimatorInstancing(string name);
    ~DissolveModelAnimatorInstancing();

    void Update();
    void Render();
    void GUIRender();

    Transform* Add();

    void PlayClip(UINT instanceID, int clip, float scale = 1.0f, float takeTime = 0.05f);
    void SetDissolve(UINT index, float value) { dissolveInstancingBuffer->Get().dissolves[index] = value; }

    Matrix GetTransformByNode(UINT instanceID, int nodeIndex);

    Motion* GetMotion(UINT instanceID)
    {
        return &frameInstancingBuffer->Get().motions[instanceID];
    }

    UINT GetClipSize() { return clips.size(); }

private:
    void UpdateFrame(UINT instanceID, Motion& motion);
    void UpdateTransforms();

private:
    vector<Transform*> transforms;
    DissolveInstanceData instanceDatas[MAX_INSTANCE];

    VertexBuffer* instanceBuffer;
    FrameInstancingBuffer* frameInstancingBuffer;
    DissolveInstancingBuffer* dissolveInstancingBuffer;

    Texture* dissolveMap = nullptr;

    UINT drawCount = 0;
};