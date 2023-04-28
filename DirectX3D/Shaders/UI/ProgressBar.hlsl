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
    float prevAmount;
}

Texture2D backImage : register(t10);

float4 PS(PixelInput input) : SV_TARGET
{	
	if (input.uv.x < fillAmount)
		return diffuseMap.Sample(samp, input.uv);
	else if (input.uv.x < prevAmount)
        return diffuseMap.Sample(samp, input.uv) * float4(1.0f, 0.1f, 0.1f, 1.0f);

	return backImage.Sample(samp, input.uv);
}