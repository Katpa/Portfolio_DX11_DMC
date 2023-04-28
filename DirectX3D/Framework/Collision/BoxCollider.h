#pragma once

class BoxCollider : public Collider
{
public:
    struct ObbDesc
    {
        Vector3 pos;
        Vector3 axis[3];
        Vector3 halfSize;
    };

public:
    BoxCollider(Vector3 size = Vector3(1, 1, 1));
    ~BoxCollider() = default;
        
    virtual bool IsRayCollision(IN Ray ray, OUT Contact* contact) override;

    virtual bool IsBoxCollision(BoxCollider* collider) override;
    virtual bool IsSphereCollision(SphereCollider* collider) override;
    virtual bool IsCapsuleCollision(CapsuleCollider* collider) override;

    void GetObb(ObbDesc& obbDesc);

    float R() { return GlobalPos().x + size.x * 0.5f; }
    float L() { return GlobalPos().x - size.x * 0.5f; }
    float F() { return GlobalPos().z + size.z * 0.5f; }
    float B() { return GlobalPos().z - size.z * 0.5f; }

private:
    bool IsSeperateAxis(Vector3 D, Vector3 axis, ObbDesc box1, ObbDesc box2);

private:
    virtual void MakeMesh() override;

private:
    Vector3 size;
};