#include "Framework.h"

Empusa::Empusa(Transform* transform, DissolveModelAnimatorInstancing* instancing, UINT index)
    :Enemy(transform, instancing, 100.0f), index(index)
{
    btBody = new Transform();
    
    body = new CapsuleCollider();
    body->SetParent(btBody);
    body->Scale() = { 0.5f, 0.75f, 0.5f };
    body->SetActive(false);

    Vector3 size = { 100, 100, 150 };
    arm = new BoxCollider(size);
    arm->SetParent(this->transform);
    arm->Pos().z -= 125.0f;
    arm->Pos().y += size.y + 10.0f;
    ArmOff();

    moveCollider = new CapsuleCollider();
    moveCollider->SetParent(transform);
    moveCollider->Scale() = { 70.0f, 50.0f, 70.0f };
    moveCollider->Pos().y += 50.0f;

    dissolve.curTime = 0.0f;
    dissolve.duration = 5.0f;

    SetEvents();
}

Empusa::~Empusa()
{
    delete btBody;
    delete body;
    delete arm;
    delete parry;
    delete moveCollider;
}

void Empusa::Update()
{
    if (!transform->Active()) return;

    Enemy::Update();
    AIHandler();

    BlowAway();

    switch (state.curState)
    {
    case DEATH:
        Dead();
        break;

    case ATK:
        ATtack();
        break;

    case RUN:
        Move();
        break;

    case HIT:
        SetFallingLanding();
        break;
    }

    btBody->SetWorld(instancing->GetTransformByNode(index, 12));
    body->UpdateWorld();
    arm->UpdateWorld();
    moveCollider->UpdateWorld();

    newPathTime += DELTA;
    if (newPathTime > 2.0f)
    {
        newPathTime -= 1.0f;
        GetPath(index);
    }
}

void Empusa::Render()
{
    if (!transform->Active()) return;

    moveCollider->Render();
    body->Render();
    if(arm->Active())
        arm->Render();
    //parry->Render();
}

void Empusa::Spawn(const Vector3& spawnPos)
{
    transform->SetActive(true);
    body->SetActive(true);
    
    SetAnim(SPAWN);

    state.status.SetInit();
    state.status.Update();
    lockOnImage->SetAmount(state.status.HPRate());

    transform->Pos() = spawnPos;

    path.clear();
    PATH_FIND->SetPathReady(index);

    dissolve.curTime = 0.0f;
    instancing->SetDissolve(index, dissolve.DissolveValue());
}

void Empusa::TakeHit(float dmg, HitType type)
{
    if (IsDead()) return;

    state.hitType = type;

    SetAnim(IDLE);
    if (!state.status.GetDmg(dmg))
        SetAnim(DEATH);
    else
        SetAnim(HIT);

    state.status.Update();
    lockOnImage->SetAmount(state.status.HPRate());
}

void Empusa::AIHandler()
{
    if (state.curState != IDLE) return;
    idleTime -= DELTA;
    if (idleTime > 0.0f) return;
    
    if ((ENEMY->TargetPos() - transform->Pos()).Length() > attackLimit)
    {
        if (path.size() > 0)
            SetAnim(RUN);
        else
            SetIdle();
    }
    else
    {
        SetAnim(ATK);
    }
}

void Empusa::Dead()
{
    dissolve.Set();
    instancing->SetDissolve(index, dissolve.DissolveValue());

    ENEMY->EmpusaDead();

    if (transform->Pos().y < 10)
        transform->Pos().y = 0.0f;
}

void Empusa::GetHit(float dmg)
{
    state.status.GetDmg(dmg);
}

void Empusa::Move()
{
    if (pathIndex >= path.size())
        pathIndex = 0;

    if (path.size() > 100)
    {
        path.clear();
        return;
    }

    if (path.size() == 0) return;
    Vector3 tmp = path[pathIndex];
    float dist = (transform->GlobalPos() - tmp).Length();
    Vector3 direction = (tmp - transform->GlobalPos()).GetNormalized();
    Vector3 forward = transform->Forward();
    Vector3 back = transform->Back();

    //===========================================

    Rotate(forward, direction);

    transform->Pos() += back * moveSpeed * DELTA;
    //===========================================

    if (dist < moveLimit)
    {
        pathIndex++;
        if (pathIndex == path.size())
        {
            direction = (ENEMY->TargetPos() - transform->GlobalPos()).GetNormalized();
            Rotate(forward, direction);

            dist = (ENEMY->TargetPos() - transform->GlobalPos()).Length();
            if (dist < attackLimit)
                SetAnim(ATK);
            else
                SetIdle();
            path.clear();
        }
    }
}

void Empusa::ATtack()
{
    if (arm->Active())
    {
        if (!(arm->IsCollision(ENEMY->ReturnTarget()->ReturnBody()))) return;
        
        ENEMY->ReturnTarget()->SetTakeHit(state.dmg);
        ArmOff();
    }
}

void Empusa::BlowAway()
{
    if (!isBlow) return;

    transform->Pos().x += blowDirection.x * DELTA;
    transform->Pos().z += blowDirection.z * DELTA;

    MoveCollision(moveCollider);
}

void Empusa::SetIdle()
{
    SetAnim(State::IDLE);
    idleTime = Random(1.0f, 5.0f);
}

void Empusa::SetFalling()
{
    curAnim = TakeHit_falling;
    instancing->PlayClip(index, curAnim, 1.5f);
    eventIters[curAnim] = totalEvent[curAnim].begin();
}

void Empusa::SetFallingLanding()
{
    if (blowDirection.y > 0) return;
    if (transform->Pos().y > 10.0f || transform->Pos().y == 0.0f) return;

    transform->Pos().y = 0.0f;

    isInAir = false;

    curAnim = TakeHit_falling_Landing;
    eventIters[curAnim] = totalEvent[curAnim].begin();
    instancing->PlayClip(index, curAnim, 1.5f, 0.02f);
}

void Empusa::SetLaunchingLoop()
{
    curAnim = TakeHit_Launcing_loop;
    eventIters[curAnim] = totalEvent[curAnim].begin();
    instancing->PlayClip(index, curAnim, 1.5f);
}

void Empusa::SetLaunchingEnd()
{
    blowDirection *= 0.25f;
    BlowOff();

    curAnim = TakeHit_Launcing_end;
    eventIters[curAnim] = totalEvent[curAnim].begin();
    instancing->PlayClip(index, curAnim, 1.5f);
}

void Empusa::GetUP()
{
    curAnim = GetUp;
    eventIters[curAnim] = totalEvent[curAnim].begin();
    instancing->PlayClip(index, curAnim, 1.5f);
}

void Empusa::SetDeath()
{
    transform->SetActive(false);
    body->SetActive(false);
}

void Empusa::SetEvents()
{
    motion = instancing->GetMotion(index);
    totalEvent.resize(instancing->GetClipSize());
    eventIters.resize(instancing->GetClipSize());

    SetEvent(Spawn_A, bind(&Empusa::SetIdle, this), 0.8f);
    SetEvent(Spawn_B, bind(&Empusa::SetIdle, this), 0.8f);
    SetEvent(Attack, bind(&Enemy::RotateDirectly, this), 0.01f);
    SetEvent(Attack, bind(&Empusa::ArmOn, this), 0.35f);
    SetEvent(Attack, bind(&Empusa::ArmOff, this), 0.42f);
    SetEvent(Attack, bind(&Empusa::SetIdle, this), 0.85f);
    SetEvent(TakeHit_F, bind(&Empusa::BlowOn, this), 0.1f);
    SetEvent(TakeHit_F, bind(&Empusa::BlowOff, this), 0.2f);
    SetEvent(TakeHit_F, bind(&Empusa::SetIdle, this), 0.85f);
    SetEvent(TakeHit_B, bind(&Empusa::BlowOn, this), 0.1f);
    SetEvent(TakeHit_B, bind(&Empusa::BlowOff, this), 0.2f);
    SetEvent(TakeHit_B, bind(&Empusa::SetIdle, this), 0.85f);
    SetEvent(TakeHit_R, bind(&Empusa::BlowOn, this), 0.1f);
    SetEvent(TakeHit_R, bind(&Empusa::BlowOff, this), 0.2f);
    SetEvent(TakeHit_R, bind(&Empusa::SetIdle, this), 0.85f);
    SetEvent(TakeHit_L, bind(&Empusa::BlowOn, this), 0.1f);
    SetEvent(TakeHit_L, bind(&Empusa::BlowOff, this), 0.2f);
    SetEvent(TakeHit_L, bind(&Empusa::SetIdle, this), 0.85f);
    SetEvent(TakeHit_air, bind(&Empusa::SetFalling, this), 0.85f);
    SetEvent(TakeHit_falling_Landing, bind(&Empusa::BlowOn, this), 0.01f);
    SetEvent(TakeHit_falling_Landing, bind(&Empusa::GetUP, this), 0.8f);
    SetEvent(TakeHit_Launcing_start, bind(&Empusa::SetLaunchingLoop, this), 0.8f);
    SetEvent(TakeHit_Launcing_loop, bind(&Empusa::SetLaunchingEnd, this), 0.8f);
    SetEvent(TakeHit_Launcing_end, bind(&Empusa::GetUP, this), 0.8f);
    SetEvent(GetUp, bind(&Empusa::SetIdle, this), 0.8f);
    SetEvent(Death, bind(&Empusa::SetDeath, this), 0.85f);
}

void Empusa::SetAnim(Enemy::State action)
{
    if (state.curState == action) return;

    state.curState = action;
    
    switch (state.curState)
    {
    case Empusa::SPAWN:
        SetAnimSpawn();
        break;
    case Empusa::IDLE:
        SetAnimIdle();
        break;
    case Empusa::ATK:
        SetAnimAtk();
        break;
    case Empusa::RUN:
        SetAnimRun();
        break;
    case Empusa::DEATH:
        SetAnimDeath();
        break;
    case Empusa::HIT:
        SetAnimTakeHit();
        break;
    }

    if (state.curState != GET_DOWN)
    {
        eventIters[curAnim] = totalEvent[curAnim].begin();
        instancing->PlayClip(index, curAnim, 1.5f);
    }
}

void Empusa::SetEvent(int clip, Event event, float timeRatio)
{
    if (totalEvent[clip].count(timeRatio) > 0)
        return;

    totalEvent[clip][timeRatio] = event;
}

void Empusa::SetAnimSpawn()
{
    int randNum = Random(0, 2);
    if (randNum)
        curAnim = Spawn_A;
    else
        curAnim = Spawn_B;
    
}

void Empusa::SetAnimIdle()
{
    int randNum = Random(0, 3);
    switch (randNum)
    {
    case 0:
        curAnim = Idle_A;
        break;
    case 1:
        curAnim = Idle_B;
        break;
    case 2:
        curAnim = Idle_C;
        break;
    }
}

void Empusa::SetAnimAtk()
{
    curAnim = Attack;
}

void Empusa::SetAnimRun()
{
    curAnim = Run;
}

void Empusa::SetAnimDeath()
{
    if (state.prevState == GET_DOWN)
        curAnim = Death_downward;
    else
        curAnim = Death;

    BlowOff();
}

void Empusa::SetAnimTakeHit()
{
    blowDirection = {};

    switch (state.hitType)
    {
    case HitType::NORMAL:
    {
        Vector3 dir = (transform->GlobalPos() - ENEMY->ReturnTarget()->GlobalPos()).GetNormalized();

        float f = (transform->Forward() - dir).Length();
        float b = (transform->Back() - dir).Length();
        float r = (transform->Right() - dir).Length();
        float l = (transform->Left() - dir).Length();

        isBlow = true;

        float minValue = min(f, min(b, min(r, l)));
        if (isInAir)
        {
            curAnim = Action::TakeHit_air;
            blowDirection.y = 100.0f;
        }
        else if (minValue == f)     curAnim = Action::TakeHit_F;
        else if (minValue == b)     curAnim = Action::TakeHit_B;
        else if (minValue == r)     curAnim = Action::TakeHit_R;
        else                        curAnim = Action::TakeHit_L;

        blowDirection = dir * 60.0f;

        break;
    }
    case HitType::UPPER:
    {
        RotateDirectly();
        curAnim = Action::TakeHit_air;
        blowDirection.y = 1100.0f;
        isInAir = true;
        break;
    }
    case HitType::SLAM:
    {

        break;
    }
    case HitType::LAUNCH:
    {
        RotateDirectly();
        transform->UpdateWorld();
        curAnim = Action::TakeHit_Launcing_start;
        blowDirection = transform->Forward() * 300.0f;
        isBlow = true;
        break;
    }
    default:
        break;
    }
}