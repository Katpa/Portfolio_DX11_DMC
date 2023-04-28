struct InputDesc
{
    float3 aUp;
    float3 aPos;
    float aHeight;
    float aRadius;
    
    float3 bUp;
    float3 bPos;
    float bHeight;
    float bRadius;
};
StructuredBuffer<InputDesc> input : register(t0);

struct OutputDesc
{
    int isCollision;
};
RWStructuredBuffer<OutputDesc> output : register(u0);

float3 ClosestPointOnLine(float3 start, float3 end, float3 p)
{
    float3 line = end - start;
    float3 A = p -start;

    float x =   dot(line, A);
    float y = dot(line, line);

    float t = clamp(0, 1, x / y);

    return start + line * t;
}

void CapsuleCollision(uint index)
{
    float3 aDir = input[index].aUp;
    float3 aA = input[index].aPos - aDir * input[index].aHeight * 0.5f;
    float3 aB = input[index].aPos + aDir * input[index].aHeight * 0.5f;
    
    float3 bDir = input[index].bUp;
    float3 bA = input[index].bPos - bDir * input[index].bHeight * 0.5f;
    float3 bB = input[index].bPos + bDir * input[index].bHeight * 0.5f;
    
    float3 v0 = bA - aA;
    float3 v1 = bB - aA;
    float3 v2 = bA - aB;
    float3 v3 = bB - aB;
    
    float d0 = dot(v0, v0);
    float d1 = dot(v1, v1);
    float d2 = dot(v2, v2);
    float d3 = dot(v3, v3);
    
    float3 bestA;
    [flatten]
    if (d2 < d0 || d2 < d1 || d3 < d0 || d3 > d1)
        bestA = aB;
    else
        bestA = aA;
    
    float3 bestB = ClosestPointOnLine(bA, bB, bestA);
    bestA = ClosestPointOnLine(aA, aB, bestB);
    bestB = ClosestPointOnLine(bA, bB, bestA);
    
    float distance = distance(bestA, bestB);
    
    if (distance <= input[index].aRadius + input[index].bRadius)
        output[index].isCollision = 1;
    else
        output[index].isCollision = 0;
}

[numthreads(64, 1, 1)]
void CS( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x;
    
    // 그룹 수 비례해서 할 수 있어야한다.
    // 그룹이 2개면 스레드 ID는 127까지 있는다
    // 64 자리엔 총 검사해야할 횟수가 들어가야 한다
    if(64 > index)
        CapsuleCollision(index);
}