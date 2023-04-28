#include "Framework.h"
#include "Dante.h"

Dante::Dante()
    :Actor(100.0f, DANTE_MOVE_SPEED)
{
    CreateModels();
    ReadAnims();
    maBody->SetParent(this);
    maBody->Rot().y += XM_PI;
    CAM->SetTarget(this);
    CAM->TargetOptionLoad("DMC_TPS");

    body = new CapsuleCollider(35.0f, 120.0f);
    body->SetParent(this);
    body->Pos().y += 90.0f;

    UI->SetPlayerUI(status.CurHP());
}

Dante::~Dante()
{
    delete maBody;
    delete mHead;
    delete mHair;

    delete btHead;
    delete btRightHand;
    delete btLeftHand;
    delete btWeaponIdle;

    delete reb;
    delete rebCollider;
    delete trail;
    delete rebTrailStart;
    delete rebTrailEnd;

    delete body;
}

void Dante::Update()
{
    Control();
    ActionUpdate();

    btRightHand->SetWorld(maBody->GetTransformByNode(298));
    btLeftHand->SetWorld(maBody->GetTransformByNode(135));
    btWeaponIdle->SetWorld(maBody->GetTransformByNode(449));
    btHead->SetWorld(maBody->GetTransformByNode(20));

    Transform::UpdateWorld();
    maBody->Update();
    
    reb->UpdateWorld();
    rebCollider->UpdateWorld();
    if (combo.IsCombo())
    {
        rebTrailStart->Pos() = rebCollider->GlobalPos() - rebCollider->Up() * 100.0f;
        rebTrailEnd->Pos() = rebCollider->GlobalPos() + rebCollider->Up() * 100.0f;
        rebTrailStart->UpdateWorld();
        rebTrailEnd->UpdateWorld();
        trail->Update();
    }
    
    //cer->Update();
    mHead->UpdateWorld();
    mHair->UpdateWorld();
    btRightHand->UpdateWorld();
    btLeftHand->UpdateWorld();
    btHead->UpdateWorld();

    body->UpdateWorld();

    rebLight->active = reb->Active();
    rebLight->pos = rebCollider->GlobalPos();
}

void Dante::Render()
{
    maBody->Render();
    mHead->Render();
    mHair->Render();

    reb->Render();
    rebCollider->Render();
    if(combo.IsCombo())
       trail->Render();

    body->Render();
}

void Dante::SetTakeHit(float dmg)
{
    if (!status.GetDmg(dmg))
    {
        SetAnim(CAST(UINT, Action::Death));
    }
    else if (jump.IsJump())
    {
        SetAnim(CAST(UINT, Action::TakeHit_Air));
        jump.TakeHit();
    }
    else
        SetAnim(CAST(UINT, Action::TakeHit));
    SetAbleInput2();
}

void Dante::Control()
{
    // Attack()?
    // Melee()?

    ActionMove();

    LockOn();
    Jump();
    Dash();
    Move();

    Input();

    AttackControl();
}

void Dante::CreateModels()
{
    maBody = new ModelAnimator("Dante_body");
    maBody->SetParent(this);

    mHead = new Model("Dante_head");
    mHair = new Model("Dante_hair");
    btRightHand = new Transform();
    btLeftHand = new Transform();
    btWeaponIdle = new Transform();
    btHead = new Transform();

    mHead->SetParent(btHead);
    mHead->SetTag("Head");
    mHead->Load();
    mHair->SetParent(btHead);
    mHair->SetTag("Head");
    mHair->Load();


    //무기 설정
    reb = new Model("Rebellion");
    reb->SetParent(btWeaponIdle);
    reb->SetTag("Rebellion");
    rebCollider = new CapsuleCollider(0.1f, 1.0f);
    rebCollider->SetParent(reb);
    rebCollider->Rot().x += XM_PIDIV2;
    rebCollider->Pos().z -= 0.85f;
    rebTrailStart = new Transform();
    rebTrailEnd = new Transform();
    trail = new Trail(L"Textures/Effect/RebTrail.png", rebTrailStart, rebTrailEnd, 80, 200.0f);
    trail->SetColor(0.35f, 0.35f, 0.35f);
    //trail->SetColor(0.5f, 0.5f, 0.5f);

    cer = new ModelAnimator("Cerberus");
    cer->SetTag("Cerberus");
    cer->Load();
    cer->SetParent(btWeaponIdle);

    CreateWeaponLight();
}

void Dante::CreateWeaponLight()
{
    rebLight = Environment::Get()->AddLight();
    rebLight->type = 1;
    rebLight->color.x = 182.0f / 255.0f;
    rebLight->color.y = 34.0f / 255.0f;
    rebLight->color.z = 34.0f / 255.0f;
    rebLight->range = 200.0f;
}

void Dante::ReadAnims()
{
    // Dante
    //-------------------------------------------------------
    maBody->ReadClip("Idle");
    //maBody->GetClip(CAST(int, Action::IDLE))->SetEvent(bind(&Dante::WeaponSetIdle, this), 0.5f);
    maBody->ReadClip("Idle_normal");
    //maBody->GetClip(CAST(int, Action::IDLE_NORMAL))->SetEvent(bind(&Dante::WeaponSetIdle, this), 0.01f);
    maBody->ReadClip("Idle_lockOn");
    //maBody->GetClip(CAST(int, Action::IDLE_LOCKON))->SetEvent(bind(&Dante::WeaponSetIdle, this), 0.01f);
    maBody->ReadClip("Lock_walk_F");
    maBody->GetClip(CAST(int, Action::LOCK_WALK_F))->SetEvent(bind(&Dante::SetStateInit, this), 0.5f);
    maBody->ReadClip("Lock_walk_B");
    maBody->GetClip(CAST(int, Action::LOCK_WALK_B))->SetEvent(bind(&Dante::SetStateInit, this), 0.5f);
    maBody->ReadClip("Lock_walk_R");
    maBody->GetClip(CAST(int, Action::LOCK_WALK_R))->SetEvent(bind(&Dante::SetStateInit, this), 0.5f);
    maBody->ReadClip("Lock_walk_L");
    maBody->GetClip(CAST(int, Action::LOCK_WALK_L))->SetEvent(bind(&Dante::SetStateInit, this), 0.5f);
    maBody->ReadClip("Run");
    maBody->GetClip(CAST(int, Action::RUN))->SetEvent(bind(&Dante::SetStateInit, this), 0.5f);
    maBody->ReadClip("Dash_F");
    maBody->GetClip(CAST(int, Action::Dash_F))->SetEvent(bind(&Dante::SetAbleInput1, this), 0.35f);
    maBody->GetClip(CAST(int, Action::Dash_F))->SetEvent(bind(&Dante::SetIdle, this), 0.75f);
    maBody->ReadClip("Dash_B");
    maBody->GetClip(CAST(int, Action::Dash_B))->SetEvent(bind(&Dante::SetAbleInput1, this), 0.35f);
    maBody->GetClip(CAST(int, Action::Dash_B))->SetEvent(bind(&Dante::SetIdle, this), 0.75f);
    maBody->ReadClip("Dash_R");
    maBody->GetClip(CAST(int, Action::Dash_R))->SetEvent(bind(&Dante::SetAbleInput1, this), 0.35f);
    maBody->GetClip(CAST(int, Action::Dash_R))->SetEvent(bind(&Dante::SetIdle, this), 0.75f);
    maBody->ReadClip("Dash_L");
    maBody->GetClip(CAST(int, Action::Dash_L))->SetEvent(bind(&Dante::SetAbleInput1, this), 0.35f);
    maBody->GetClip(CAST(int, Action::Dash_L))->SetEvent(bind(&Dante::SetIdle, this), 0.75f);
    maBody->ReadClip("Skystar");
    maBody->GetClip(CAST(int, Action::Skystar))->SetEvent(bind(&Dante::SetAbleInput1, this), 0.25f);
    maBody->GetClip(CAST(int, Action::Skystar))->SetEvent(bind(&Dante::SetIdle, this), 0.75f);

    maBody->ReadClip("Evade_R");
    maBody->GetClip(CAST(int, Action::Evade_R))->SetEvent(bind(&Dante::SetIdle, this), 1.0f);
    maBody->ReadClip("Evade_L");
    maBody->GetClip(CAST(int, Action::Evade_L))->SetEvent(bind(&Dante::SetIdle, this), 1.0f);

    maBody->ReadClip("Jump_start");
    maBody->GetClip(CAST(int, Action::Jump_start))->SetEvent(bind(&Dante::SetJumpLoop, this), 0.8f);
    maBody->ReadClip("Jump_loop"); 
    maBody->ReadClip("Jump_end");
    maBody->GetClip(CAST(int, Action::Jump_end))->SetEvent(bind(&Dante::SetAbleInput0, this), 0.01f);
    maBody->GetClip(CAST(int, Action::Jump_end))->SetEvent(bind(&Dante::SetLanding, this), 0.02f);
    maBody->GetClip(CAST(int, Action::Jump_end))->SetEvent(bind(&Dante::SetIdle, this), 0.8f);

    maBody->ReadClip("EnemyStep");
    //maBody->GetClip(2)->SetEvent(bind(&ModelAnimationScene::SetEvent, this), 0.65f);       에너미스텝

    maBody->ReadClip("Airtrick_start");
    maBody->GetClip(CAST(int, Action::Airtrick_start))->SetEvent(bind(&Dante::SetAirTrick, this), 0.4f);

    maBody->ReadClip("Airtrick_ground");
    maBody->GetClip(CAST(int, Action::Airtrick_ground))->SetEvent(bind(&Dante::SetAbleInput1, this), 0.05f);
    maBody->GetClip(CAST(int, Action::Airtrick_ground))->SetEvent(bind(&ComboHandler::ComboEnd, &combo), 0.1f);
    maBody->GetClip(CAST(int, Action::Airtrick_ground))->SetEvent(bind(&Dante::SetIdle, this), 0.65f);

    maBody->ReadClip("Airtrick_air");
    maBody->GetClip(CAST(int, Action::Airtrick_air))->SetEvent(bind(&Dante::SetAbleInput0, this), 0.05f);
    maBody->GetClip(CAST(int, Action::Airtrick_air))->SetEvent(bind(&ComboHandler::ComboEnd, &combo), 0.1f);
    maBody->GetClip(CAST(int, Action::Airtrick_air))->SetEvent(bind(&Dante::SetIdle, this), 0.65f);

    maBody->ReadClip("GetUp");
    
    maBody->ReadClip("TakeHit_Air");
    maBody->GetClip(CAST(int, Action::TakeHit_Air))->SetEvent(bind(&Dante::SetIdle, this), 0.8f);
    maBody->ReadClip("TakeHit");
    maBody->GetClip(CAST(int, Action::TakeHit))->SetEvent(bind(&Dante::SetIdle, this), 0.8f);
    
    maBody->ReadClip("Taunt");
    maBody->GetClip(CAST(int, Action::Taunt))->SetEvent(bind(&Dante::SetIdle, this), 0.8f);
    maBody->ReadClip("Death");
    
    // Rebellion anim set
    maBody->ReadClip("Reb_Melee_A");
    maBody->GetClip(CAST(int, Action::Reb_Melee_A))->SetEvent(bind(&Dante::WeaponSetRightHand, this), 0.01f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_A))->SetEvent(bind(&Dante::OnAllEnemyBody, this), 0.03f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_A))->SetEvent(bind(&ComboHandler::SetInit, &combo), 0.1f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_A))->SetEvent(bind(&Dante::SetAbleInput1, this), 0.15f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_A))->SetEvent(bind(&ComboHandler::SwapState, &combo), 0.17f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_A))->SetEvent(bind(&ComboHandler::SwapState, &combo), 0.25f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_A))->SetEvent(bind(&ComboHandler::SwapState, &combo), 0.35f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_A))->SetEvent(bind(&MoveHandler::ActionOn, &move), 0.02f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_A))->SetEvent(bind(&MoveHandler::ActionOff, &move), 0.12f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_A))->SetEvent(bind(&Dante::WeaponSetIdle, this), 0.64f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_A))->SetEvent(bind(&Dante::SetAbleInput0, this), 0.65f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_A))->SetEvent(bind(&Dante::SetIdle, this), 0.6f);
    combos[CAST(int, Action::Reb_Melee_A)] = { CAST(UINT, Action::Reb_Melee_B), CAST(UINT, Action::Reb_Melee_D) };


    maBody->ReadClip("Reb_Melee_B");
    maBody->GetClip(CAST(int, Action::Reb_Melee_B))->SetEvent(bind(&Dante::WeaponSetRightHand, this), 0.01f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_B))->SetEvent(bind(&Dante::OnAllEnemyBody, this), 0.02f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_B))->SetEvent(bind(&Dante::SetAbleInput1, this), 0.1f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_B))->SetEvent(bind(&ComboHandler::SetInit, &combo), 0.13f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_B))->SetEvent(bind(&ComboHandler::SwapState, &combo), 0.16f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_B))->SetEvent(bind(&ComboHandler::SwapState, &combo), 0.5f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_B))->SetEvent(bind(&MoveHandler::ActionOn, &move), 0.02f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_B))->SetEvent(bind(&MoveHandler::ActionOff, &move), 0.09f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_B))->SetEvent(bind(&Dante::SetAbleInput0, this), 0.6f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_B))->SetEvent(bind(&Dante::WeaponSetIdle, this), 0.65f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_B))->SetEvent(bind(&Dante::SetIdle, this), 0.7f);
    combos[CAST(int, Action::Reb_Melee_B)] = { CAST(UINT, Action::Reb_Melee_C), 0 };


    maBody->ReadClip("Reb_Melee_C");
    maBody->GetClip(CAST(int, Action::Reb_Melee_C))->SetEvent(bind(&Dante::WeaponSetRightHand, this), 0.01f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_C))->SetEvent(bind(&Dante::OnAllEnemyBody, this), 0.02f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_C))->SetEvent(bind(&ComboHandler::SetInit, &combo), 0.13f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_C))->SetEvent(bind(&Dante::SetAbleInput1, this), 0.15f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_C))->SetEvent(bind(&MoveHandler::ActionOn, &move), 0.03f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_C))->SetEvent(bind(&MoveHandler::ActionOff, &move), 0.11f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_C))->SetEvent(bind(&Dante::SetAbleInput0, this), 0.55f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_C))->SetEvent(bind(&Dante::WeaponSetIdle, this), 0.65f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_C))->SetEvent(bind(&Dante::SetIdle, this), 0.7f);
    combos[CAST(int, Action::Reb_Melee_C)] = { 0, 0 };


    maBody->ReadClip("Reb_Melee_D");
    maBody->GetClip(CAST(int, Action::Reb_Melee_D))->SetEvent(bind(&Dante::WeaponSetLeftHand, this), 0.01f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_D))->SetEvent(bind(&Dante::OnAllEnemyBody, this), 0.02f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_D))->SetEvent(bind(&Dante::SetAbleInput1, this), 0.15f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_D))->SetEvent(bind(&ComboHandler::SetInit, &combo), 0.17f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_D))->SetEvent(bind(&ComboHandler::SwapState, &combo), 0.4f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_D))->SetEvent(bind(&ComboHandler::SwapState, &combo), 0.7f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_D))->SetEvent(bind(&MoveHandler::ActionOn, &move), 0.03f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_D))->SetEvent(bind(&MoveHandler::ActionOff, &move), 0.16f);
    //maBody->GetClip(CAST(int, Action::Reb_Melee_D))->SetEvent(bind(&Dante::OnAllEnemyBody, this), 0.25f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_D))->SetEvent(bind(&Dante::WeaponSetRightHand, this), 0.37f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_D))->SetEvent(bind(&Dante::SetAbleInput0, this), 0.45f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_D))->SetEvent(bind(&Dante::WeaponSetIdle, this), 0.85f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_D))->SetEvent(bind(&Dante::SetIdle, this), 0.9f);
    combos[CAST(int, Action::Reb_Melee_D)] = { CAST(UINT, Action::Reb_Melee_E), 0 };


    maBody->ReadClip("Reb_Melee_E");
    maBody->GetClip(CAST(int, Action::Reb_Melee_E))->SetEvent(bind(&Dante::WeaponSetRightHand, this), 0.01f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_E))->SetEvent(bind(&Dante::OnAllEnemyBody, this), 0.02f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_E))->SetEvent(bind(&Dante::SetAbleInput1, this), 0.15f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_E))->SetEvent(bind(&ComboHandler::SetInit, &combo), 0.17f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_E))->SetEvent(bind(&ComboHandler::SwapState, &combo), 0.2f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_E))->SetEvent(bind(&ComboHandler::SwapState, &combo), 0.8f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_E))->SetEvent(bind(&MoveHandler::PowerActionOn, &move), 0.02f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_E))->SetEvent(bind(&MoveHandler::PowerActionOff, &move), 0.18f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_E))->SetEvent(bind(&Dante::WeaponSetIdle, this), 0.85f);
    maBody->GetClip(CAST(int, Action::Reb_Melee_E))->SetEvent(bind(&Dante::SetIdle, this), 0.9f);
    combos[CAST(int, Action::Reb_Melee_E)] = { CAST(UINT, Action::Reb_MillionStab_loop), 0 };


    maBody->ReadClip("Reb_Air_Melee_A");
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_A))->SetEvent(bind(&Dante::WeaponSetRightHand, this), 0.01f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_A))->SetEvent(bind(&Dante::OnAllEnemyBody, this), 0.02f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_A))->SetEvent(bind(&Dante::SetAbleInput1, this), 0.15f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_A))->SetEvent(bind(&ComboHandler::SetInit, &combo), 0.17f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_A))->SetEvent(bind(&ComboHandler::SwapState, &combo), 0.2f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_A))->SetEvent(bind(&ComboHandler::SwapState, &combo), 0.8f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_A))->SetEvent(bind(&Dante::WeaponSetIdle, this), 0.85f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_A))->SetEvent(bind(&Dante::SetIdle, this), 0.9f);
    combos[CAST(int, Action::Reb_Air_Melee_A)] = { CAST(UINT, Action::Reb_Air_Melee_B), 0 };


    maBody->ReadClip("Reb_Air_Melee_B");
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_B))->SetEvent(bind(&Dante::WeaponSetRightHand, this), 0.01f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_B))->SetEvent(bind(&Dante::OnAllEnemyBody, this), 0.02f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_B))->SetEvent(bind(&Dante::SetAbleInput1, this), 0.15f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_B))->SetEvent(bind(&ComboHandler::SetInit, &combo), 0.17f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_B))->SetEvent(bind(&ComboHandler::SwapState, &combo), 0.2f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_B))->SetEvent(bind(&ComboHandler::SwapState, &combo), 0.8f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_B))->SetEvent(bind(&Dante::WeaponSetIdle, this), 0.85f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_B))->SetEvent(bind(&Dante::SetIdle, this), 0.9f);
    combos[CAST(int, Action::Reb_Air_Melee_B)] = { CAST(UINT, Action::Reb_Air_Melee_C), 0 };


    maBody->ReadClip("Reb_Air_Melee_C");
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_C))->SetEvent(bind(&Dante::WeaponSetRightHand, this), 0.01f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_C))->SetEvent(bind(&Dante::OnAllEnemyBody, this), 0.02f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_C))->SetEvent(bind(&Dante::SetAbleInput1, this), 0.15f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_C))->SetEvent(bind(&ComboHandler::SetInit, &combo), 0.17f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_C))->SetEvent(bind(&ComboHandler::SwapState, &combo), 0.2f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_C))->SetEvent(bind(&ComboHandler::SwapState, &combo), 0.8f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_C))->SetEvent(bind(&Dante::SetAbleInput0, this), 0.7f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_C))->SetEvent(bind(&Dante::WeaponSetIdle, this), 0.85f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_C))->SetEvent(bind(&Dante::SetIdle, this), 0.9f);
    combos[CAST(int, Action::Reb_Air_Melee_C)] = { CAST(UINT, Action::Reb_Air_Melee_D), 0 };


    maBody->ReadClip("Reb_Air_Melee_D");
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_D))->SetEvent(bind(&Dante::WeaponSetRightHand, this), 0.01f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_D))->SetEvent(bind(&Dante::OnAllEnemyBody, this), 0.02f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_D))->SetEvent(bind(&Dante::SetAbleInput1, this), 0.15f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_D))->SetEvent(bind(&ComboHandler::SetInit, &combo), 0.17f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_D))->SetEvent(bind(&ComboHandler::SwapState, &combo), 0.2f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_D))->SetEvent(bind(&ComboHandler::SwapState, &combo), 0.8f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_D))->SetEvent(bind(&Dante::WeaponSetIdle, this), 0.85f);
    maBody->GetClip(CAST(int, Action::Reb_Air_Melee_D))->SetEvent(bind(&Dante::SetIdle, this), 0.9f);
    combos[CAST(int, Action::Reb_Air_Melee_D)] = { 0, 0 };


    maBody->ReadClip("Reb_MillionStab_loop");
    maBody->GetClip(CAST(int, Action::Reb_MillionStab_loop))->SetEvent(bind(&Dante::WeaponSetRightHand, this), 0.01f);
    maBody->GetClip(CAST(int, Action::Reb_MillionStab_loop))->SetEvent(bind(&Dante::OnAllEnemyBody, this), 0.02f);
    maBody->GetClip(CAST(int, Action::Reb_MillionStab_loop))->SetEvent(bind(&Dante::SetAbleInput1, this), 0.15f);
    maBody->GetClip(CAST(int, Action::Reb_MillionStab_loop))->SetEvent(bind(&ComboHandler::SetInit, &combo), 0.17f);
    maBody->GetClip(CAST(int, Action::Reb_MillionStab_loop))->SetEvent(bind(&ComboHandler::SwapState, &combo), 0.2f);
    maBody->GetClip(CAST(int, Action::Reb_MillionStab_loop))->SetEvent(bind(&Dante::OnAllEnemyBody, this), 0.22f);
    maBody->GetClip(CAST(int, Action::Reb_MillionStab_loop))->SetEvent(bind(&Dante::OnAllEnemyBody, this), 0.42f);
    maBody->GetClip(CAST(int, Action::Reb_MillionStab_loop))->SetEvent(bind(&Dante::OnAllEnemyBody, this), 0.62f);
    maBody->GetClip(CAST(int, Action::Reb_MillionStab_loop))->SetEvent(bind(&ComboHandler::SwapState, &combo), 0.85f);
    maBody->GetClip(CAST(int, Action::Reb_MillionStab_loop))->SetEvent(bind(&Dante::MillionStabEnd, this), 0.9f);
    combos[CAST(int, Action::Reb_MillionStab_loop)] = { CAST(int, Action::Reb_MillionStab_loop), 0 };


    maBody->ReadClip("Reb_MillionStab_end");
    maBody->GetClip(CAST(int, Action::Reb_MillionStab_end))->SetEvent(bind(&Dante::WeaponSetRightHand, this), 0.01f);
    maBody->GetClip(CAST(int, Action::Reb_MillionStab_end))->SetEvent(bind(&Dante::OnAllEnemyBody, this), 0.12f);
    maBody->GetClip(CAST(int, Action::Reb_MillionStab_end))->SetEvent(bind(&Dante::SetAbleInput1, this), 0.15f);
    maBody->GetClip(CAST(int, Action::Reb_MillionStab_end))->SetEvent(bind(&ComboHandler::SetInit, &combo), 0.17f);
    maBody->GetClip(CAST(int, Action::Reb_MillionStab_end))->SetEvent(bind(&Dante::SetAbleInput0, this), 0.65f);
    maBody->GetClip(CAST(int, Action::Reb_MillionStab_end))->SetEvent(bind(&MoveHandler::PowerActionOn, &move), 0.1f);
    maBody->GetClip(CAST(int, Action::Reb_MillionStab_end))->SetEvent(bind(&MoveHandler::PowerActionOff, &move), 0.14f);
    maBody->GetClip(CAST(int, Action::Reb_MillionStab_end))->SetEvent(bind(&Dante::WeaponSetIdle, this), 0.68f);
    maBody->GetClip(CAST(int, Action::Reb_MillionStab_end))->SetEvent(bind(&Dante::SetIdle, this), 0.7f);
    combos[CAST(int, Action::Reb_MillionStab_end)] = { 0, 0 };


    maBody->ReadClip("Reb_Parring");


    maBody->ReadClip("Reb_HighTime");
    maBody->GetClip(CAST(int, Action::Reb_HighTime))->SetEvent(bind(&Dante::WeaponSetRightHand, this), 0.01f);
    maBody->GetClip(CAST(int, Action::Reb_HighTime))->SetEvent(bind(&Dante::OffAllEnemyBody, this), 0.02f);
    maBody->GetClip(CAST(int, Action::Reb_HighTime))->SetEvent(bind(&Dante::OnAllEnemyBody, this), 0.03f);
    maBody->GetClip(CAST(int, Action::Reb_HighTime))->SetEvent(bind(&ComboHandler::ComboEnd, &combo), 0.04f);
    maBody->GetClip(CAST(int, Action::Reb_HighTime))->SetEvent(bind(&ComboHandler::ComboStart, &combo), 0.06f);
    maBody->GetClip(CAST(int, Action::Reb_HighTime))->SetEvent(bind(&Dante::HighTime, this), 0.15f);
    maBody->GetClip(CAST(int, Action::Reb_HighTime))->SetEvent(bind(&Dante::SetAbleInput1, this), 0.5f);
    maBody->GetClip(CAST(int, Action::Reb_HighTime))->SetEvent(bind(&Dante::WeaponSetIdle, this), 0.68f);
    maBody->GetClip(CAST(int, Action::Reb_HighTime))->SetEvent(bind(&Dante::SetIdle, this), 0.7f);
    combos[CAST(int, Action::Reb_HighTime)] = { 0, 0 };


    maBody->ReadClip("Reb_HighTime_jump");
    maBody->GetClip(CAST(int, Action::Reb_HighTime_jump))->SetEvent(bind(&Dante::WeaponSetRightHand, this), 0.01f);
    maBody->GetClip(CAST(int, Action::Reb_HighTime_jump))->SetEvent(bind(&Dante::SetIdle, this), 0.7f);
    combos[CAST(int, Action::Reb_HighTime_jump)] = { 0, 0 };


    maBody->ReadClip("Reb_HelmBreaker_start");
    maBody->GetClip(CAST(int, Action::Reb_HelmBreaker_start))->SetEvent(bind(&Dante::WeaponSetRightHand, this), 0.01f);
    maBody->GetClip(CAST(int, Action::Reb_HelmBreaker_start))->SetEvent(bind(&Dante::OnAllEnemyBody, this), 0.02f);
    maBody->GetClip(CAST(int, Action::Reb_HelmBreaker_start))->SetEvent(bind(&Dante::HelmBreaker, this), 0.45f);
    combos[CAST(int, Action::Reb_HelmBreaker_start)] = { 0, 0 };
    

    maBody->ReadClip("Reb_HelmBreaker_loop");
    maBody->GetClip(CAST(int, Action::Reb_HelmBreaker_loop))->SetEvent(bind(&Dante::HelmBreaker, this), 0.45f);
    combos[CAST(int, Action::Reb_HelmBreaker_loop)] = { 0, 0 };


    maBody->ReadClip("Reb_HelmBreaker_end");
    maBody->GetClip(CAST(int, Action::Reb_HelmBreaker_end))->SetEvent(bind(&Dante::OnAllEnemyBody, this), 0.02f);
    maBody->GetClip(CAST(int, Action::Reb_HelmBreaker_end))->SetEvent(bind(&Dante::SetStateInit, this), 0.2f);
    maBody->GetClip(CAST(int, Action::Reb_HelmBreaker_end))->SetEvent(bind(&Dante::SetAbleInput0, this), 0.23f);
    maBody->GetClip(CAST(int, Action::Reb_HelmBreaker_end))->SetEvent(bind(&Dante::SetIdle, this), 0.5f);
    combos[CAST(int, Action::Reb_HelmBreaker_end)] = { 0, 0 };


    maBody->ReadClip("Reb_Stinger_start");
    maBody->GetClip(CAST(int, Action::Reb_Stinger_start))->SetEvent(bind(&Dante::WeaponSetRightHand, this), 0.01f);
    maBody->GetClip(CAST(int, Action::Reb_Stinger_start))->SetEvent(bind(&Dante::OnAllEnemyBody, this), 0.3f);
    maBody->GetClip(CAST(int, Action::Reb_Stinger_start))->SetEvent(bind(&Dante::StingerToLoop, this), 0.15f);

    maBody->ReadClip("Reb_Stinger_loop");

    maBody->ReadClip("Reb_Stinger_end");
    maBody->GetClip(CAST(int, Action::Reb_Stinger_end))->SetEvent(bind(&Dante::SetAbleInput0, this), 0.4f);
    maBody->GetClip(CAST(int, Action::Reb_Stinger_end))->SetEvent(bind(&ComboHandler::ComboEnd, &combo), 0.6f);
    maBody->GetClip(CAST(int, Action::Reb_Stinger_end))->SetEvent(bind(&Dante::SetIdle, this), 0.7f);

    /*maBody->ReadClip("Cer_Melee_A");
    maBody->ReadClip("Cer_Melee_B");
    maBody->ReadClip("Cer_Melee_C");
    maBody->ReadClip("Cer_Melee_D");
    maBody->ReadClip("Cer_Melee_E");
    maBody->ReadClip("Cer_Melee_F");
    maBody->ReadClip("Cer_Melee_G_start");
    maBody->ReadClip("Cer_Melee_G_loop");
    maBody->ReadClip("Cer_Melee_G_end");
    maBody->ReadClip("Cer_Revolver_start");
    maBody->ReadClip("Cer_Revolver_loop");
    maBody->ReadClip("Cer_Revolver_end");
    maBody->ReadClip("Cer_Parring");
    maBody->ReadClip("Cer_Air_Melee");
    maBody->ReadClip("Cer_Crystal");*/
    // //maBody->GetClip(1)->SetEvent(bind(&ModelAnimationScene::SetIdle, this), 0.4f); revolver start -> loop
    //-------------------------------------------------------

    // Cerberus
    //-------------------------------------------------------
    //cer->ReadClip("Cer_Idle");
    //cer->ReadClip("Cer_Melee_A");
    //cer->ReadClip("Cer_Melee_B");
    //cer->ReadClip("Cer_Melee_C");
    //cer->ReadClip("Cer_Melee_D");
    //cer->ReadClip("Cer_Melee_E");
    //cer->ReadClip("Cer_Melee_F");
    //cer->ReadClip("Cer_Melee_G_start");
    //cer->ReadClip("Cer_Melee_G_loop");
    //cer->ReadClip("Cer_Melee_G_end");
    //cer->ReadClip("Cer_Revolver_start");
    //cer->ReadClip("Cer_Revolver_loop");
    //cer->ReadClip("Cer_Revolver_end");
    //cer->ReadClip("Cer_Parring");
    //cer->ReadClip("Cer_Air_Melee");
    //cer->ReadClip("Cer_Crystal");
    //cer->GetClip(1)->SetEvent(bind(&ModelAnimationScene::SetIdle, this), 0.8f); //cer가 Idle로 돌아가야하는 타이밍
    //-------------------------------------------------------
}


//------------------------------------------------------------------------
// Code: void Input()
// Desc: input keys for controling character
//------------------------------------------------------------------------
void Dante::Input()
{
    // 여기는 제어 불능을 위한 예외
    /// ==========================
    if (ableInput > 2) return;
    
    if (KEY_DOWN(CAST(int, Cmd::LOCKON)) || PAD_DOWN(CAST(int, PadCmd::LOCKON)))
    {
        lockOnTarget = ENEMY->ReturnLockOnTarget(GlobalPos());

        if (state == CAST(int, Action::IDLE))
            SetIdle();
    }
    else if (KEY_UP(CAST(int, Cmd::LOCKON)) || PAD_UP(CAST(int, PadCmd::LOCKON)))
    {
        ENEMY->LockOff();
        lockOnTarget = nullptr;

        if(state == CAST(int, Action::IDLE_LOCKON))
            SetIdle();
    }

    /// ==========================
    if (ableInput > 1) return;
    //여기에 점프 및 공격 다른 동작들 넣기

    if (KEY_DOWN(CAST(int, Cmd::JUMP)) || PAD_DOWN(CAST(int, PadCmd::JUMP)))
    {
        if (!jump.IsJump())
        {
            SetAnim(CAST(UINT, Action::Jump_start));
            jump.Init();
            SetAbleInput1();
        }
        else
        {
            //enemyStep
            // 나도 적도 낙하속도 0으로 만들어야함
        }
    }

    if (KEY_DOWN(CAST(int, Cmd::STYLE)) || PAD_DOWN(CAST(int, PadCmd::STYLE)))
    {
        SetAnim(CAST(UINT, Action::IDLE));

        dash.Init();
        if (move.Input())
        {
            move.SetDirection();

            if (lockOnTarget)
            {
                combo.SetInit();

                UINT dir = move.IsLockTightDir(GlobalPos(), lockOnTarget->GlobalPos(), Right());

                if (dir == 1)
                {
                    isAirTrickAir = true;
                    SetAnim(CAST(UINT, Action::Airtrick_start));
                    SetAbleInput2();
                    ableInput = 2;
                }
                else if (dir == 2)
                {
                    isAirTrickAir = false;
                    SetAnim(CAST(UINT, Action::Airtrick_start));
                    SetAbleInput2();
                    ableInput = 2;
                }
                else
                {
                    move.SetDirectRotate(Rot());
                    Rot().y = move.ReturnTargetRotateY();
                    dash.SetDir(move.ReturnDirection());
                }
            }
            else
            {
                move.SetDirectRotate(Rot());
                Rot().y = move.ReturnTargetRotateY();
                dash.SetDir(move.ReturnDirection());
            }
        }
        else
        {
            dash.SetDir(this->Forward());
        }

        if (lockOnTarget)
        {
        }
        else if (jump.IsJump())
        {
            SetAnim(CAST(UINT, Action::Skystar));
            jump.SkystarInit();
            ableInput = 2;
        }
        else
        {
            SetAnim(CAST(UINT, Action::Dash_F));
            ableInput = 2;
        }
    }

    if (KEY_DOWN(CAST(int, Cmd::MELEE)) || PAD_DOWN(CAST(int, PadCmd::MELEE)))
    {
        if (move.Input() && lockOnTarget)
        {
            move.SetDirection();
            UINT dir = move.LockInputDir();

            if (dir == 1 && !jump.IsJump())
            {
                SetAnim(CAST(UINT, Action::Reb_Stinger_start));

                combo.ComboStart();
                ableInput = 2;
            }
            else if (dir == 2)
            {
                if (jump.IsJump())
                {
                    SetAnim(CAST(UINT, Action::Reb_HelmBreaker_start));
                }
                else
                {
                    SetAnim(CAST(UINT, Action::Reb_HighTime));
                }

                combo.ComboStart();
                ableInput = 2;
            }
        }

        if (combo.NCombo())
        {
            if (combos[state].Normal())
            {
                SetAnim(combos[state].Normal());
                combo.CloseNCombo();
                ableInput = 2;
            }
        }
        else if (combo.UCombo())
        {
            if (combos[state].Unique())
            {
                SetAnim(combos[state].Unique());
                combo.CloseUCombo();
                ableInput = 2;
            }
        }
        else if (!combo.IsCombo())
        {
            if (jump.IsJump())
            {
                SetAnim(CAST(UINT, Action::Reb_Air_Melee_A));
                jump.SkystarInit();
            }
            else
            {
                SetAnim(CAST(UINT, Action::Reb_Melee_A));
            }
            combo.ComboStart();
            ableInput = 2;
        }
    }

    /// ==========================
    if (ableInput > 0) return;

    if (KEY_PRESS(CAST(UINT, Cmd::TAUNT)) || PAD_DOWN(CAST(int, PadCmd::TAUNT)))
    {
        SetAnim(CAST(UINT, Action::Taunt));
    }

    if (move.Input() && !jump.IsJump())
    {
        if (lockOnTarget)
        {
            move.SetDirection();

            Vector3 dir = move.ReturnDirection();
            Vector2 input = move.ReturnInput();
            
            float dist = (dir - Forward()).Length();
            
            if (fabs(input.y) >= fabs(input.x))
            {
                if (input.y > 0)
                    SetAnim(CAST(UINT, Action::LOCK_WALK_F));
                else
                    SetAnim(CAST(UINT, Action::LOCK_WALK_B));
            }
            else
            {
                if (input.x > 0)
                    SetAnim(CAST(UINT, Action::LOCK_WALK_R));
                else
                    SetAnim(CAST(UINT, Action::LOCK_WALK_L));
            }

            move.Move(Pos(), true);

            SetLookTarget();
            MoveCollision();
        }
        else
            SetAnim(CAST(UINT, Action::RUN));
    }
    else if (state == CAST(UINT, Action::RUN) ||
             state == CAST(UINT, Action::LOCK_WALK_F) ||
             state == CAST(UINT, Action::LOCK_WALK_B) ||
             state == CAST(UINT, Action::LOCK_WALK_R) ||
             state == CAST(UINT, Action::LOCK_WALK_L))
    {
        if (lockOnTarget)
            SetAnim(CAST(UINT, Action::IDLE_LOCKON));
        else
            SetAnim(CAST(UINT, Action::IDLE));
    }
}

void Dante::ActionUpdate()
{
    if (!combo.IsCombo()) return;

    if (state == CAST(UINT, Action::Reb_Stinger_loop))
    {
        if ((lockOnTarget->Pos() - Pos()).Length() > TILE_HALFSIZE && !IsMoveCollision())
        {
            SetLookTarget();
            Pos() += Forward() * DANTE_MOVE_SPEED * 3.0f * DELTA;
        }
        else
        {
            SetAnim(CAST(UINT, Action::Reb_Stinger_end));
        }
    }
}

void Dante::ActionMove()
{
    if (move.IsAction())
        move.ActionMove(Pos(), Forward());
    else if (move.IsPowerAction())
        move.PowerActionMove(Pos(), Forward());
}

//------------------------------------------------------------------------
// Code: void Jump()
// Desc: Handle character's transform and state during jump
//------------------------------------------------------------------------
void Dante::Jump()
{
    if (!jump.IsJump()) return;

    if (state == CAST(UINT, Action::Jump_start) ||
        state == CAST(UINT, Action::Jump_loop) ||
        state == CAST(UINT, Action::Reb_HighTime_jump))
    {
        jump.InJump(Pos());

        if (Pos().y < 0)
        {
            Pos().y = 0;
            SetAnim(CAST(UINT, Action::Jump_end));
        }

        if (move.Input() && state != CAST(UINT, Action::Reb_HighTime_jump))
        {
            move.SetDirection();
            move.AirMove(Pos());
            move.Rotate(Rot(), Back());

            MoveCollision();
        }

        SetStateInit();
    }
    else if (state == CAST(UINT, Action::Reb_HelmBreaker_loop))
    {
        if(Pos().y > 3)
            jump.HelmBreakerLoop(Pos());
        else
        {
            Pos().y = 0;
            jump.EndJump();
            SetAnim(CAST(UINT, Action::Reb_HelmBreaker_end));
        }
    }
    else
    {
        if(Pos().y > 0)
            jump.ActionInJump(Pos());
        else
        {
            Pos().y = 0;
            jump.EndJump();
        }
    }

    if (lockOnTarget)
        SetLookTarget();
}

void Dante::Dash()
{
    if (state != CAST(UINT, Action::Dash_F) && state != CAST(UINT, Action::Skystar)) return;

    Pos() += dash.DashToDir();

    if (jump.IsJump() && jump.SkystarInJump(Pos()))
    {
        move.Input();
        move.SetDirection();
        move.AirMove(Pos());
        move.Rotate(Rot(), Back());
    }

    MoveCollision();
}

void Dante::Move()
{
    if (state != CAST(UINT, Action::RUN)) return;

    if (!move.Input())
    {
        SetAnim(CAST(UINT, Action::IDLE));
        return;
    }

    move.SetDirection();
    move.Move(Pos());
    move.Rotate(Rot(), Back());
    MoveCollision();
}

void Dante::LockOn()
{
    if (!lockOnTarget) return;
    
    Vector3 lockOnPos = ENEMY->LockOnPos();

    if (lockOnPos.x > CENTER_X + 75)
        CAM->Rot().y += CAM_ROT_SPEED * DELTA;
    else if (lockOnPos.x < CENTER_X - 75)
        CAM->Rot().y -= CAM_ROT_SPEED * DELTA;

    if (lockOnPos.y > CENTER_Y + 50)
        CAM->Rot().x -= DELTA;
    else if (lockOnPos.y < CENTER_Y - 50)
        CAM->Rot().x += DELTA;

    CAM->SetPosByRotY();
}

void Dante::AttackControl()
{
    if (!combo.IsCombo()) return;

    Enemy* hitTarget = nullptr;
    
    if (wType == REB)
    {
        switch (state)
        {
            // NORMAL
        case CAST(UINT, Action::Reb_Melee_A):
        case CAST(UINT, Action::Reb_Melee_B):
        case CAST(UINT, Action::Reb_Air_Melee_A):
        case CAST(UINT, Action::Reb_Air_Melee_B):
        case CAST(UINT, Action::Reb_Air_Melee_C):
        case CAST(UINT, Action::Reb_Melee_D):
        case CAST(UINT, Action::Reb_Melee_E):
            ENEMY->IsTakeHitCollisionInOut(rebCollider, rebDmg, HitType::NORMAL);
            break;

        case CAST(UINT, Action::Reb_MillionStab_loop):
            ENEMY->IsTakeHitCollisionJust(rebCollider, rebDmg, HitType::NORMAL);
            break;

            //UPPER
        case CAST(UINT, Action::Reb_HighTime):
        case CAST(UINT, Action::Reb_HighTime_jump):
            ENEMY->IsTakeHitCollisionInOut(rebCollider, rebDmg, HitType::UPPER);
            break;

            //LAUNCH
        case CAST(UINT, Action::Reb_Melee_C):
        case CAST(UINT, Action::Reb_Air_Melee_D):
            ENEMY->IsTakeHitCollisionInOut(rebCollider, rebDmg, HitType::LAUNCH);
            break;

        case CAST(UINT, Action::Reb_MillionStab_end):
        case CAST(UINT, Action::Reb_Stinger_end):
        case CAST(UINT, Action::Reb_HelmBreaker_end):
            ENEMY->IsTakeHitCollisionJust(rebCollider, rebDmg, HitType::LAUNCH);
            break;

            //SLAM
        case CAST(UINT, Action::Reb_HelmBreaker_start):
            ENEMY->IsTakeHitCollisionJust(rebCollider, rebDmg, HitType::SLAM);
            break;
        //case CAST(UINT, Action::Reb_Parring):
        //case CAST(UINT, Action::Reb_HelmBreaker_loop):
        //case CAST(UINT, Action::Reb_Stinger_start):
        //case CAST(UINT, Action::Reb_Stinger_loop):
        }
    }
}

void Dante::MoveCollision()
{
    dungeon->CheckPlayer(Pos());
    collidingWall = dungeon->IsWallCollision(body);

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

bool Dante::IsMoveCollision()
{
    dungeon->CheckPlayer(Pos());
    if (collidingWall = dungeon->IsWallCollision(body))
        return true;

    return false;
}

void Dante::SetLookTarget()
{
    Vector3 dir = (lockOnTarget->Pos() - Pos()).GetNormalized();
    Rot().y = atan2(dir.x, dir.z);
}

void Dante::SetIdle()
{
    if (jump.IsJump())
    {
        SetAnim(CAST(int, Action::Jump_loop), 1.5f, 0.1f);
        SetAbleInput1();
    }
    else
    {
        if (lockOnTarget)
            SetAnim(CAST(UINT, Action::IDLE_LOCKON), 1.5f, 0.1f);
        else
            SetAnim(CAST(UINT, Action::IDLE), 1.5f, 0.1f);
        SetAbleInput0();
    }

    WeaponSetIdle();
    SetStateInit();
}

void Dante::SetJumpLoop()
{
    SetAnim(CAST(int, Action::Jump_loop), 1.5f, 0.1f);
}

void Dante::SetLanding()
{
    jump.EndJump();
}

void Dante::MillionStabLoop()
{
    SetAnim(CAST(int, Action::IDLE), 1.5f, 0.1f);
    SetAnim(CAST(int, Action::Reb_MillionStab_loop), 1.5f, 0.1f);
}

void Dante::MillionStabEnd()
{
    SetAnim(CAST(int, Action::Reb_MillionStab_end), 1.5f, 0.1f);
}

void Dante::SetAirTrick()
{
    Vector3 dir = (lockOnTarget->Pos() - Pos()).GetNormalized();
    float dist = (lockOnTarget->Pos() - Pos()).Length();

    if (dist <= 100.0f)
        dist = 100.0f;

    if (isAirTrickAir)
    {
        SetAnim(CAST(UINT, Action::Airtrick_air));
        
        jump.Init();
        jump.SkystarInit();

        Pos() += dir * (dist - 100);
        Pos().y += 50;
    }
    else
    {
        SetAnim(CAST(UINT, Action::Airtrick_ground));

        jump.EndJump();

        Pos() += dir * (dist - 100);
        Pos().y = 0;
    }

    move.DIR() = dir;
    move.SetDirectRotate(Rot());
}

void Dante::HighTime()
{
    if (KEY_PRESS(CAST(UINT, Cmd::MELEE)) || PAD_PRESS(CAST(UINT, PadCmd::MELEE)))
    {
        SetAnim(CAST(UINT, Action::Reb_HighTime_jump));
        jump.Init();
    }
}

void Dante::HelmBreaker()
{
    if (state == CAST(UINT, Action::Reb_HelmBreaker_start))
    {
        SetAnim(CAST(UINT, Action::Reb_HelmBreaker_loop));
        jump.Init();
        jump.SkystarInit();
    }
}

void Dante::StingerToLoop()
{
    if (state == CAST(UINT, Action::Reb_Stinger_start))
    {
        SetAnim(CAST(UINT, Action::Reb_Stinger_loop));
    }
}

void Dante::OnAllEnemyBody()
{
    ENEMY->AllActivatedEnemyBodyOn();
}

void Dante::OffAllEnemyBody()
{
    ENEMY->AllActivatedEnemyBodyOff();
}

void Dante::WeaponSetIdle()
{
    if (reb->Active())
        reb->SetParent(btWeaponIdle);
    else
        cer->SetParent(btWeaponIdle);
}

void Dante::WeaponSetRightHand()
{
    if (reb->Active())
        reb->SetParent(btRightHand);
    else
        cer->SetParent(btRightHand);
}

void Dante::WeaponSetLeftHand()
{
    reb->SetParent(btLeftHand);
}

void Dante::SetStateInit()
{
    combo.SetInit();
    combo.ComboEnd();
    move.ActionOff();
}