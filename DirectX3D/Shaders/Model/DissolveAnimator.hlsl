#include "../VertexHeader.hlsli"
#include "../PixelHeader.hlsli"

cbuffer ValueBuffer : register(b10)
{
    float3 color;
    float fillAmount;
}

LightPixelInput VS(VertexUVNormalTangentBlend input)
{
    LightPixelInput output;
    matrix transform = mul(SkinWorld(input.indices, input.weights), world);
	
    output.pos = mul(input.pos, transform);
	
    output.viewPos = invView._41_42_43;
    output.worldPos = output.pos;
	
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);

    output.uv = input.uv;
	
    output.normal = mul(input.normal, (float3x3) transform);
    output.tangent = mul(input.tangent, (float3x3) transform);
    output.binormal = cross(output.normal, output.tangent);
	
    return output;
}

Texture2D DissolveMap : register(t11);

float4 PS(LightPixelInput input) : SV_TARGET
{
    Material material = GetMaterial(input);
	
    float4 albedo;
    albedo.rgb = color;
    
    float4 emissive = mEmissive;
    
    float4 dissolve = DissolveMap.Sample(samp, input.uv);
    
    [flatten]
    if (dissolve.r > fillAmount + 0.1f)
        albedo = 0;
    else if (dissolve.r > fillAmount)
    {
        float gradient = dissolve.r - fillAmount;
        
        albedo.r = albedo.r - gradient * 5.0f;
        albedo.g = albedo.g - gradient * 2.5f;
        //emissive.r = 0.5f;
        //emissive.g = 0.0f;
        //emissive.b = 0.0f;
        //emissive.w = 0.2f;
    }
    else
    {
        [unroll(MAX_LIGHT)]
        for (int i = 0; i < lightCount; i++)
        {
		[flatten]
            if (!lights[i].active)
                continue;

		[flatten]
            if (lights[i].type == 0)
                albedo += CalcDirectional(material, lights[i]);
            else if (lights[i].type == 1)
                albedo += CalcPoint(material, lights[i]);
            else if (lights[i].type == 2)
                albedo += CalcSpot(material, lights[i]);
            else if (lights[i].type == 3)
                albedo += CalcCapsule(material, lights[i]);
        }
    }
    
    float4 ambient = CalcAmbient(material);
	
    return albedo + ambient + emissive;
}