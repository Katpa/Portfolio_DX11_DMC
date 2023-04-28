#include "Framework.h"
#include "ModelAnimationScene.h"

ModelAnimationScene::ModelAnimationScene()
{
    modelAnimator = new ModelAnimator("Urizen");

    trans = new Transform();
    collider = new CapsuleCollider();
    collider->SetParent(trans);


    // Urizen
    //-------------------------------------------------------
    modelAnimator->ReadClip("Idle");
    modelAnimator->ReadClip("Death");
    modelAnimator->ReadClip("Walk");

    modelAnimator->ReadClip("Battle_start");
    modelAnimator->ReadClip("Attack1");
    modelAnimator->ReadClip("Attack1_end");
    modelAnimator->ReadClip("Attack2");
    modelAnimator->ReadClip("Attack2_end");
    modelAnimator->ReadClip("Assult_strat");
    modelAnimator->ReadClip("Assult_loop");
    modelAnimator->ReadClip("Assult_end");
    //-------------------------------------------------------

    //rightHand = new Transform();
    //weaponIdle = new Transform();
    //cer = new ModelAnimator("Cerberus");
    //cer->SetTag("Cerberus");
    //cer->Load();
    //cer->SetParent(rightHand);
    //reb = new Model("Rebellion");
    //reb->SetParent(rightHand);
    //reb->SetTag("Rebellion");
    //
    //bHead = new Transform();
    //head = new Model("Dante_head");
    //head->SetParent(bHead);
    //head->SetTag("Head");
    //head->Load();
    //hair = new Model("Dante_hair");
    //hair->SetParent(bHead);
    //hair->SetTag("Head");
    //hair->Load();

    // Empusa
    //-------------------------------------------------------
    //modelAnimator->ReadClip("Idle_A");
    //modelAnimator->ReadClip("Idle_B");
    //modelAnimator->ReadClip("Idle_C");

    //modelAnimator->ReadClip("Attack");
    //modelAnimator->ReadClip("TakeHit_F");
    //modelAnimator->ReadClip("TakeHit_B");
    //modelAnimator->ReadClip("TakeHit_R");
    //modelAnimator->ReadClip("TakeHit_L");

    //modelAnimator->ReadClip("Death");
    //modelAnimator->ReadClip("TakeHit_air");
    //modelAnimator->ReadClip("Spawn_A");
    //modelAnimator->ReadClip("Spawn_B");
    //modelAnimator->ReadClip("Run");
    //
    //modelAnimator->ReadClip("TakeHit_Launcing_start");
    //modelAnimator->ReadClip("TakeHit_Launcing_loop");
    //modelAnimator->ReadClip("TakeHit_Launcing_end");
    //
    //modelAnimator->ReadClip("TakeHit_downward");
    //modelAnimator->ReadClip("Death_downward");
    //modelAnimator->ReadClip("TakeHit_Slam_start");
    //modelAnimator->ReadClip("TakeHit_Slam_loop");
    //modelAnimator->ReadClip("TakeHit_Slam_end");
    //modelAnimator->ReadClip("TakeHit_fallingFloat"); 
    ////modelAnimator->GetClip(1)->SetEvent(bind(&ModelAnimationScene::SetIdle, this), 0.05f);        slam start->loop
    //modelAnimator->ReadClip("TakeHit_falling");
    //modelAnimator->ReadClip("TakeHit_falling_Landing");

    //modelAnimator->ReadClip("GetUp");

    //-------------------------------------------------------

    // GreenEmpusa
    //-------------------------------------------------------
    //modelAnimator->ReadClip("Idle_A");
    //modelAnimator->ReadClip("Idle_B");
    //modelAnimator->ReadClip("Run");

    //modelAnimator->ReadClip("Attack_charge_start");
    //modelAnimator->ReadClip("Attack_charge_loop");
    //modelAnimator->ReadClip("Attack_charge_end");

    //modelAnimator->ReadClip("TakeHit_air");
    //modelAnimator->ReadClip("TakeHit_launching_start");
    //modelAnimator->ReadClip("TakeHit_launching_loop");
    //modelAnimator->ReadClip("TakeHit_launching_end");
    //modelAnimator->GetClip(1)->SetEvent(bind(&ModelAnimationScene::SetIdle, this), 0.87f); launching start -> loop

    //modelAnimator->ReadClip("TakeHit_downward");
    //modelAnimator->ReadClip("TakeHit_slam_start");
    //modelAnimator->ReadClip("TakeHit_slam_loop");
    //modelAnimator->ReadClip("TakeHit_slam_end");
    //modelAnimator->ReadClip("TakeHit_fallingFloat");
    //modelAnimator->GetClip(1)->SetEvent(bind(&ModelAnimationScene::SetIdle, this), 0.05f);        slam start->loop

    //modelAnimator->ReadClip("Death_downward");
    //modelAnimator->ReadClip("Spawn_A");
    //modelAnimator->ReadClip("Spawn_B");
    //-------------------------------------------------------

    // Dante
    //-------------------------------------------------------
    //modelAnimator->ReadClip("Idle");
    //modelAnimator->ReadClip("Idle_normal");
    //modelAnimator->ReadClip("Idle_lockOn");
    //modelAnimator->ReadClip("Lock_walk_F");
    //modelAnimator->ReadClip("Lock_walk_B");
    //modelAnimator->ReadClip("Lock_walk_R");
    //modelAnimator->ReadClip("Lock_walk_L");
    //modelAnimator->ReadClip("Run");
    //
    //modelAnimator->ReadClip("Dash_F");
    //modelAnimator->ReadClip("Dash_B");
    //modelAnimator->ReadClip("Dash_R");
    //modelAnimator->ReadClip("Dash_L");
    //modelAnimator->ReadClip("Evade_R");
    //modelAnimator->ReadClip("Evade_L");
    //modelAnimator->ReadClip("EnemyStep");
    //modelAnimator->GetClip(2)->SetEvent(bind(&ModelAnimationScene::SetEvent, this), 0.65f);       에너미스텝
    //modelAnimator->ReadClip("Airtrick_start");
    //modelAnimator->GetClip(3)->SetEvent(bind(&ModelAnimationScene::SetEvent, this), 0.4f);        에어트릭
    //modelAnimator->ReadClip("Airtrick_ground");
    //modelAnimator->ReadClip("Airtrick_air");
    //modelAnimator->ReadClip("GetUp");
    //
    //modelAnimator->ReadClip("TakeHit_Air");
    //modelAnimator->ReadClip("TakeHit");
    //
    //modelAnimator->ReadClip("Taunt");
    //modelAnimator->ReadClip("Death");
    //
    //modelAnimator->ReadClip("Reb_Melee_A");
    //modelAnimator->ReadClip("Reb_Melee_B");
    //modelAnimator->ReadClip("Reb_Melee_C");
    //modelAnimator->ReadClip("Reb_Melee_D");
    //modelAnimator->ReadClip("Reb_Melee_E");
    //modelAnimator->ReadClip("Reb_MillionStab_loop");
    //modelAnimator->ReadClip("Reb_MillionStab_end");
    //modelAnimator->ReadClip("Reb_Parring");
    //modelAnimator->ReadClip("Reb_HighTime");
    //modelAnimator->ReadClip("Reb_HighTime_jump");
    //modelAnimator->ReadClip("Reb_Air_Melee_A");
    //modelAnimator->ReadClip("Reb_Air_Melee_B");
    //modelAnimator->ReadClip("Reb_Air_Melee_C");
    //modelAnimator->ReadClip("Reb_Air_Melee_D");
    //modelAnimator->ReadClip("Reb_HelmBreaker_start");
    ////modelAnimator->GetClip(2)->SetEvent(bind(&ModelAnimationScene::SetEvent, this), 0.45f);         헬름브레이커 start->loop
    //modelAnimator->ReadClip("Reb_HelmBreaker_loop");
    //modelAnimator->ReadClip("Reb_HelmBreaker_end");
    //modelAnimator->ReadClip("Reb_Stinger_start");
    ////modelAnimator->GetClip(2)->SetEvent(bind(&ModelAnimationScene::SetEvent, this), 0.15f);         스팅어 start->loop
    //modelAnimator->ReadClip("Reb_Stinger_loop");
    //modelAnimator->ReadClip("Reb_Stinger_end");
    
    //modelAnimator->ReadClip("Cer_Melee_A");
    //modelAnimator->ReadClip("Cer_Melee_B");
    //modelAnimator->ReadClip("Cer_Melee_C");
    //modelAnimator->ReadClip("Cer_Melee_D");
    //modelAnimator->ReadClip("Cer_Melee_E");
    //modelAnimator->ReadClip("Cer_Melee_F");
    //modelAnimator->ReadClip("Cer_Melee_G_start");
    //modelAnimator->ReadClip("Cer_Melee_G_loop");
    //modelAnimator->ReadClip("Cer_Melee_G_end");
    //modelAnimator->ReadClip("Cer_Revolver_start");
    //modelAnimator->ReadClip("Cer_Revolver_loop");
    //modelAnimator->ReadClip("Cer_Revolver_end");
    //modelAnimator->ReadClip("Cer_Parring");
    //modelAnimator->ReadClip("Cer_Air_Melee");
    //modelAnimator->ReadClip("Cer_Crystal");
    // //modelAnimator->GetClip(1)->SetEvent(bind(&ModelAnimationScene::SetIdle, this), 0.4f); revolver start -> loop

    //modelAnimator->ReadClip("Jump_start");
    //modelAnimator->ReadClip("Skystar");
    //modelAnimator->ReadClip("Jump_end");

    //-------------------------------------------------------

    // Cerberus
    //-------------------------------------------------------
    //cer->ReadClip("Cer_Idle");
    //cer->ReadClip("Cer_Melee_A");
    //cer->GetClip(1)->SetEvent(bind(&ModelAnimationScene::SetIdle, this), 0.8f); //cer가 Idle로 돌아가야하는 타이밍
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
    //cer->ReadClip("Cer_Idle");
    //cer->ReadClip("Cer_Idle");
    //cer->ReadClip("Cer_Idle");
    //-------------------------------------------------------

    //modelAnimator->GetClip(1)->SetEvent(bind(&ModelAnimationScene::SetIdle, this), 0.4f);
    //modelAnimator->PlayClip(animNum, 1.5f, 0.1f);
    //cer->PlayClip(animNum, 1.5f, 0.1f);
}

ModelAnimationScene::~ModelAnimationScene()
{
    delete modelAnimator;
    delete rightHand;
    delete weaponIdle;
    delete reb;
    delete cer;

    delete head;
    delete bHead;
    delete hair;

    delete trans;
    delete collider;
}

void ModelAnimationScene::Update()
{
    if (KEY_DOWN('1') || PAD_DOWN(XINPUT_GAMEPAD_A))
    {
        animNum++;
        modelAnimator->PlayClip(animNum, 1.5f, 0.1f);
        //cer->PlayClip(animNum, 1.5f, 0.1f);

        //if (animNum)
        //    cer->SetParent(rightHand);
        //else
        //    cer->SetParent(weaponIdle);
    }
    else if (KEY_DOWN('2') || PAD_DOWN(XINPUT_GAMEPAD_B))
    {
        animNum--;
        modelAnimator->PlayClip(animNum, 1.5f, 0.1f);
        //cer->PlayClip(animNum, 1.5f, 0.1f);

        //if (animNum)
        //    cer->SetParent(rightHand);
        //else
        //    cer->SetParent(weaponIdle);
    }

    //rightHand->SetWorld(modelAnimator->GetTransformByNode(298));
    //weaponIdle->SetWorld(modelAnimator->GetTransformByNode(449));
    //bHead->SetWorld(modelAnimator->GetTransformByNode(20));

    modelAnimator->Update();

    //trans->SetWorld(modelAnimator->GetTransformByNode(12));       body
    collider->UpdateWorld();


    //reb->UpdateWorld();
    //cer->Update();
    //head->UpdateWorld();
    //hair->UpdateWorld();
    //rightHand->UpdateWorld();
    //bHead->UpdateWorld();
}

void ModelAnimationScene::PreRender()
{
}

void ModelAnimationScene::Render()
{
    modelAnimator->Render();

    collider->Render();

    //cer->Render();
    //head->Render();
    //hair->Render();
}

void ModelAnimationScene::PostRender()
{
}

void ModelAnimationScene::GUIRender()
{
    modelAnimator->GUIRender();

    collider->GUIRender();

    //cer->GUIRender();
    //head->GUIRender();
}

void ModelAnimationScene::SetIdle()
{
    //modelAnimator->PlayClip(2, 1.5f, 0.1f);
    //cer->PlayClip(0, 1.5f, 0.1f);
    //cer->SetParent(weaponIdle);
}
