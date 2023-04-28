#include "Framework.h"

Camera::Camera()
{
    tag = "Camera";

    viewBuffer = new ViewBuffer();
    viewBuffer->SetVS(1);

    Load();

    prevMousePos = mousePos;
}

Camera::~Camera()
{
    delete viewBuffer;

    Save();
}

void Camera::Update()
{
    projection = Environment::Get()->GetProjection();
    Frustum();

    if (target)
        FollowMode();
    else
        FreeMode();

    UpdateWorld();

    view = XMMatrixInverse(nullptr, world);
    viewBuffer->Set(view, world);
}

void Camera::GUIRender()
{
    if (ImGui::TreeNode("CameraOption"))
    {
        ImGui::DragFloat("MoveSpeed", &moveSpeed);
        ImGui::DragFloat("RotSpeed", &rotSpeed);

        if (target && ImGui::TreeNode("TargetOption"))
        {
            ImGui::DragFloat("Distance", &distance, 0.1f);
            ImGui::DragFloat("Height", &height, 0.1f);
            ImGui::DragFloat3("FocusOffset", (float*)&focusOffset, 0.1f);

            float degree = XMConvertToDegrees(rotY);
            ImGui::DragFloat("RotY", &degree, 0.1f);
            rotY = XMConvertToRadians(degree);

            ImGui::DragFloat("MoveDamping", &moveDamping, 0.1f);
            ImGui::DragFloat("RotDamping", &rotDamping, 0.1f);

            ImGui::Checkbox("LookAtTargetX", &isLookAtTargetX);
            ImGui::Checkbox("LookAtTargetY", &isLookAtTargetY);
            
            ImGui::InputText("File", file, 128);

            if(ImGui::Button("Save"))
                TargetOptionSave(file);
            ImGui::SameLine();
            if (ImGui::Button("Load"))
                TargetOptionLoad(file);

            ImGui::TreePop();
        }

        Transform::GUIRender();

        ImGui::TreePop();
    }
}

void Camera::SetView()
{  
    viewBuffer->SetVS(1);
    viewBuffer->SetPS(1);
}

void Camera::SetTarget(Transform* target)
{
    this->target = target;
    ClientToScreen(hWnd, &clientCenterPos);
    ShowCursor(false);
}

Vector3 Camera::ScreenToWorld(Vector3 screenPos)
{
    return XMVector3TransformCoord(screenPos, world);
}

Vector3 Camera::WorldToScreen(Vector3 worldPos)
{
    Vector3 screenPos;

    screenPos = XMVector3TransformCoord(worldPos, view);
    screenPos = XMVector3TransformCoord(screenPos, projection);
    //NDC : -1 ~ 1

    screenPos = (screenPos + Vector3::One()) * 0.5f;//0~1

    screenPos.x *= WIN_WIDTH;
    screenPos.y *= WIN_HEIGHT;

    return screenPos;
}

Ray Camera::ScreenPointToRay(Vector3 screenPoint)
{
    Vector3 screenSize(WIN_WIDTH, WIN_HEIGHT, 1.0f);

    Vector2 point;
    point.x = (screenPoint.x / screenSize.x) * 2.0f - 1.0f;
    point.y = (screenPoint.y / screenSize.y) * 2.0f - 1.0f;    

    Float4x4 temp;
    XMStoreFloat4x4(&temp, projection);

    screenPoint.x = point.x / temp._11;
    screenPoint.y = point.y / temp._22;
    screenPoint.z = 1.0f;

    screenPoint = XMVector3TransformNormal(screenPoint, world);

    Ray ray;
    ray.pos = Pos();
    ray.dir = screenPoint.GetNormalized();

    return ray;
}

void Camera::LookAtTarget()
{    
    rotMatrix = XMMatrixRotationY(target->Rot().y + rotY);

    Vector3 forward = XMVector3TransformNormal(Vector3::Forward(), rotMatrix);

    Pos() = target->GlobalPos() + forward * -distance;
    Pos().y += height;    

    Vector3 offset = XMVector3TransformCoord(focusOffset, rotMatrix);
    Vector3 targetPos = target->GlobalPos() + offset;

    Vector3 dir = (targetPos - Pos()).GetNormalized();
    forward = Vector3(dir.x, 0.0f, dir.z).GetNormalized();

    Rot().x = acos(Dot(forward, dir));
    Rot().y = atan2(dir.x, dir.z);
}

void Camera::FreeMode()
{
    Vector3 delta = mousePos - prevMousePos;
    prevMousePos = mousePos;

    if (KEY_PRESS(VK_RBUTTON))
    {
        if (KEY_PRESS('W'))
            Pos() += Forward() * moveSpeed * DELTA;
        if (KEY_PRESS('S'))
            Pos() += Back() * moveSpeed * DELTA;
        if (KEY_PRESS('A'))
            Pos() += Left() * moveSpeed * DELTA;
        if (KEY_PRESS('D'))
            Pos() += Right() * moveSpeed * DELTA;
        if (KEY_PRESS('E'))
            Pos() += Down() * moveSpeed * DELTA;
        if (KEY_PRESS('Q'))
            Pos() += Up() * moveSpeed * DELTA;

        Rot().x -= delta.y * rotSpeed * DELTA;
        Rot().y += delta.x * rotSpeed * DELTA;
    }
}

void Camera::FollowMode()
{
    Vector3 delta;

    if (PAD_CONNECT)
    {
        delta = Vector3(PAD_RS.x, PAD_RS.y, 0) * 5.0f;
    }
    else
    {
        delta = mousePos - Vector3(CENTER_X, CENTER_Y);
        SetCursorPos(clientCenterPos.x, clientCenterPos.y);
    }

    Rot().y += delta.x * rotSpeed * DELTA;
    Rot().x -= delta.y * rotSpeed * DELTA;

    if (Rot().x > CAM_MAX_ROT_X) Rot().x = CAM_MAX_ROT_X;
    else if (Rot().x < CAM_MIN_ROT_X) Rot().x = CAM_MIN_ROT_X;

    SetPosByRotY();
}

void Camera::Frustum()
{
    Float4x4 VP;
    XMStoreFloat4x4(&VP, view * projection);

    //Left
    a = VP._14 + VP._11;
    b = VP._24 + VP._21;
    c = VP._34 + VP._31;
    d = VP._44 + VP._41;
    planes[0] = XMVectorSet(a, b, c, d);

    //Right
    a = VP._14 - VP._11;
    b = VP._24 - VP._21;
    c = VP._34 - VP._31;
    d = VP._44 - VP._41;
    planes[1] = XMVectorSet(a, b, c, d);

    //Bottom
    a = VP._14 + VP._12;
    b = VP._24 + VP._22;
    c = VP._34 + VP._32;
    d = VP._44 + VP._42;
    planes[2] = XMVectorSet(a, b, c, d);

    //Top
    a = VP._14 - VP._12;
    b = VP._24 - VP._22;
    c = VP._34 - VP._32;
    d = VP._44 - VP._42;
    planes[3] = XMVectorSet(a, b, c, d);

    //Near
    a = VP._14 + VP._13;
    b = VP._24 + VP._23;
    c = VP._34 + VP._33;
    d = VP._44 + VP._43;
    planes[4] = XMVectorSet(a, b, c, d);

    //Far
    a = VP._14 - VP._13;
    b = VP._24 - VP._23;
    c = VP._34 - VP._33;
    d = VP._44 - VP._43;
    planes[5] = XMVectorSet(a, b, c, d);

    FOR(6)
        planes[i] = XMPlaneNormalize(planes[i]);
}

void Camera::TargetOptionSave(string file)
{
    string path = "TextData/Camera/" + file + ".cam";

    BinaryWriter* writer = new BinaryWriter(path);

    writer->Float(distance);
    writer->Float(height);
    writer->Float(moveDamping);
    writer->Float(rotDamping);
    writer->Float(rotY);
    writer->Vector(focusOffset);
    writer->Bool(isLookAtTargetX);
    writer->Bool(isLookAtTargetY);

    delete writer;
}

void Camera::TargetOptionLoad(string file)
{
    string path = "TextData/Camera/" + file + ".cam";

    BinaryReader* reader = new BinaryReader(path);

    distance = reader->Float();
    height = reader->Float();
    moveDamping = reader->Float();
    rotDamping = reader->Float();
    rotY = reader->Float();
    focusOffset = reader->Vector();
    isLookAtTargetX = reader->Bool();
    isLookAtTargetY = reader->Bool();

    delete reader;
}

#define TIGHT_CULLING 0.0f

bool Camera::ContainPoint(Vector3 point)
{
    FOR(6)
    {
        Vector3 dot = XMPlaneDotCoord(planes[i], point);

        // TIGHT_CULLING 값을 올릴수록 출력범위 확대
        if (dot.x < TIGHT_CULLING)
            return false;
    }

    return true;
}

void Camera::SetPosByRotY()
{
    float theta = Rot().y + XM_PIDIV2;

    Vector3 tmpVec = { cos(theta) * distance,                            // x
                       (height - 100.0f) - (sin(-Rot().x) * distance),   // y
                      -sin(theta) * distance };                           // z

    Pos() = target->GlobalPos() + tmpVec;
}
