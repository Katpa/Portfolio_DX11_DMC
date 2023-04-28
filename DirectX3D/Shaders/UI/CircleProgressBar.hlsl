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
    // �������� ����������� ������ �̿��� angle�� ����
    float2 dir = input.uv - float2(0.5f, 0.5f);
    float angle = atan2(dir.x, dir.y) + 3.14f;
    
    // ����ϴ� ���α׷������� ������ ������ �̿��� ��°� ����
    if (angle < 6.28f * fillAmount)
        return diffuseMap.Sample(samp, input.uv);

    return backImage.Sample(samp, input.uv);
}