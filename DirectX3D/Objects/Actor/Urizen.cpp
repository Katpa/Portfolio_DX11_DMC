#include "Framework.h"

#define URIZEN_MAX_HP 500.0f

Urizen::Urizen()
    :status(Status(URIZEN_MAX_HP))
{
    CreateModels();
    CreateAnimations();

    SetAnim(Idle);

    dissolveBuffer = new DissolveBuffer();
    dissolveBuffer->Get().color = { 0.6f, 0.0f, 0.6f };
    dissolve.duration = 7.0f;
    dissolveTexture = Texture::Add(L"Textures/Effect/dissolve2.jpg");
}

Urizen::~Urizen()
{
    delete urizen;
    delete body;
    delete moveCollider;
    delete rightHand;
    delete rightFoot;
    delete armCollider;
    delete footCollider;

    delete dissolveBuffer;
}

void Urizen::Update()
{
    AIHandler();
    StateUpdate();

    //rightHand->SetWorld();
    //rightFoot->SetWorld();

    Transform::UpdateWorld();
    rightHand->UpdateWorld();
    rightFoot->UpdateWorld();
    body->UpdateWorld();
    moveCollider->UpdateWorld();
    armCollider->UpdateWorld();
    footCollider->UpdateWorld();
}

void Urizen::Render()
{
    //====== Animator Render ======
    //--------- Dissolve ----------
    dissolveBuffer->SetPS(10);
    dissolveTexture->PSSet(11);
    //-----------------------------
    urizen->Render();
    //=============================

    //====== Collider Render ======
    body->Render();
    moveCollider->Render();
    armCollider->Render();
    footCollider->Render();
    //=============================
}

void Urizen::Spawn(const Vector3& spawnPos)
{
    Pos() = spawnPos;
    status.SetInit();
    SetAnim(Battle_start);
}

void Urizen::TakeHit(float dmg, HitType type)
{
    if (status.GetDmg(dmg)) return;

    SetAnim(Death);
}

void Urizen::CreateModels()
{
    urizen = new ModelAnimator("Urizen");
    urizen->SetParent(this);
    urizen->SetShader(L"Model/DissolveAnimator.hlsl");

    moveCollider = new CapsuleCollider();
    body = new CapsuleCollider();

    rightHand = new Transform();
    rightFoot = new Transform();
    
    armCollider = new CapsuleCollider();
    footCollider = new  CapsuleCollider();

    dissolveTexture = Texture::Add(L"Textures/Effect/dissolve2.jpg");
}

void Urizen::CreateAnimations()
{
    urizen->ReadClip("Idle");
    //urizen->GetClip(Idle)->SetEvent()
    urizen->ReadClip("Death");
    urizen->ReadClip("Walk");
    urizen->ReadClip("Battle_start");
    urizen->GetClip(CAST(UINT, Action::Battle_start))->SetEvent(bind(&Urizen::SetIdle, this), 0.9f);
    urizen->ReadClip("Attack1");
    urizen->GetClip(CAST(UINT, Action::Attack1))->SetEvent(bind(&Urizen::PlayAttackEnd, this), 0.9f);
    urizen->ReadClip("Attack1_end");
    urizen->GetClip(CAST(UINT, Action::Attack1_end))->SetEvent(bind(&Urizen::SetIdle, this), 0.9f);
    urizen->ReadClip("Attack2");
    urizen->GetClip(CAST(UINT, Action::Attack2))->SetEvent(bind(&Urizen::PlayAttackEnd, this), 0.9f);
    urizen->ReadClip("Attack2_end");
    urizen->GetClip(CAST(UINT, Action::Attack2_end))->SetEvent(bind(&Urizen::SetIdle, this), 0.9f);
    urizen->ReadClip("Assult_start");
    urizen->GetClip(CAST(UINT, Action::Assult_start))->SetEvent(bind(&Urizen::PlayAssultLoop, this), 0.9f);
    urizen->ReadClip("Assult_loop");
    urizen->ReadClip("Assult_end");
    urizen->GetClip(CAST(UINT, Action::Assult_end))->SetEvent(bind(&Urizen::SetIdle, this), 0.9f);
}

void Urizen::AIHandler()
{
}

void Urizen::StateUpdate()
{
    switch (curAnim)
    {
    case Urizen::Idle:
        break;
    case Urizen::Death:
        Dead();
        break;
    case Urizen::Walk:
        Move();
        break;
    case Urizen::Attack1:
        break;
    case Urizen::Attack1_end:
        break;
    case Urizen::Attack2:
        break;
    case Urizen::Attack2_end:
        break;
    case Urizen::Assult_start:
        break;
    case Urizen::Assult_loop:
        break;
    case Urizen::Assult_end:
        break;
    case Urizen::NONE:
        break;
    default:
        break;
    }
}

void Urizen::Dead()
{
    dissolve.Set();
    dissolveBuffer->Get().dissolves = dissolve.DissolveValue();
}

void Urizen::Move()
{
    Vector3 dir = (ENEMY->ReturnTarget()->GlobalPos() - GlobalPos()).GetNormalized();

    curMoveSpeed = moveSpeed * DELTA;
    if (MAX_MOVE_SPEED < curMoveSpeed)
        curMoveSpeed = MAX_MOVE_SPEED;

    Pos() += dir * curMoveSpeed;

    Rotate(dir);
}

void Urizen::Rotate(Vector3 dir)
{
    //float theta = atan2(dir.x, dir.z);
    Vector3 forward = Forward();
    Rot().y = Dot(forward, dir);
}

void Urizen::SetIdle()
{
    SetAnim(Idle);
}

void Urizen::SetAnim(Action action)
{
    if (curAnim == action) return;

    curAnim = action;

    switch (curAnim)
    {
    case Urizen::Idle:
        break;
    case Urizen::Death:
        break;
    case Urizen::Walk:
        curMoveSpeed = 0;
        break;
    case Urizen::Battle_start:
        break;
    case Urizen::Attack1:
        break;
    case Urizen::Attack1_end:
        break;
    case Urizen::Attack2:
        break;
    case Urizen::Attack2_end:
        break;
    case Urizen::Assult_start:
        break;
    case Urizen::Assult_loop:
        break;
    case Urizen::Assult_end:
        break;
    case Urizen::NONE:
        break;
    default:
        break;
    }

    urizen->PlayClip(action, 1.5f, 0.05f);
}

void Urizen::PlayAttackEnd()
{
    if (curAnim == Action::Attack1)
        SetAnim(Action::Attack1_end);
    else if (curAnim == Action::Attack2)
        SetAnim(Action::Attack2_end);
}

void Urizen::PlayAssultLoop()
{
    SetAnim(Action::Assult_loop);
}

void Urizen::MoveCollision()
{
    BoxCollider* collidingWall = ENEMY->ReturnDungeon()->IsWallCollision(body);

    if (collidingWall)
    {
        Vector3 dir = { 0, 0, 0 };

        if (collidingWall->R() < body->GlobalPos().x)
            dir.x = 1;
        else if (collidingWall->L() > body->GlobalPos().x)
            dir.x = -1;
        else if (collidingWall->F() < body->GlobalPos().z)
            dir.z = 1;
        else if (collidingWall->B() > body->GlobalPos().z)
            dir.z = -1;

        Pos() += dir * body->CollisionDepth();
    }
}
