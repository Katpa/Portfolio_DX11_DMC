#include "../VertexHeader.hlsli"
#include "../PixelHeader.hlsli"

LightPixelInput VS(VertexUVNormalTangent input)
{
    LightPixelInput output;
    output.pos = mul(input.pos, world);
	
    output.viewPos = invView._41_42_43;
    output.worldPos = output.pos;
	
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);

    output.uv = input.uv;
	
    output.normal = mul(input.normal, (float3x3) world);
    output.tangent = mul(input.tangent, (float3x3) world);
    output.binormal = cross(output.normal, output.tangent);
	
    return output;
}

float4 PS(LightPixelInput input) : SV_TARGET
{
    float4 albedo = diffuseMap.Sample(samp, input.uv);
	
    Material material = GetMaterial(input);
    material.diffuseColor = albedo;
	
	
    float4 color = CalcDirectional(material, lights[0]);
    float4 ambient = CalcAmbient(material);
    float4 emissive = CalcEmissive(material);
	
    return color + ambient + emissive;
}