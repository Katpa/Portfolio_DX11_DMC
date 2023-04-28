#include "../VertexHeader.hlsli"
#include "../PixelHeader.hlsli"

struct PixelInput
{
    float4 pos : SV_POSITION;
    float2 uv : UV;
};

PixelInput VS(VertexUV input)
{
    PixelInput output;
    output.pos = mul(input.pos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);

    output.uv = input.uv;
	
    return output;
}

cbuffer ValueBuffer : register(b10)
{
    float fillAmount;
}

Texture2D backImage : register(t10);

float4 PS(PixelInput input) : SV_TARGET
{
    // 원점에서 출력점까지의 방향을 이용해 angle값 도출
    float2 dir = input.uv - float2(0.5f, 0.5f);
    float angle = atan2(dir.x, dir.y) + 3.14f;
    
    // 출력하는 프로그래스바의 비율과 각도을 이용해 출력값 결정
    if (angle < 6.28f * fillAmount)
        return diffuseMap.Sample(samp, input.uv);

    return backImage.Sample(samp, input.uv);
}