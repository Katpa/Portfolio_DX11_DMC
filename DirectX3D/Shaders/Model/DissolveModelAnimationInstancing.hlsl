#include "../VertexHeader.hlsli"
#include "../PixelHeader.hlsli"

DissolveLightPixelInput VS(DissolveVertexInstancing input)
{
    DissolveLightPixelInput output;
    matrix transform = mul(SkinWorld(input.index, input.indices, input.weights), input.transform);
	
    output.pos = mul(input.pos, transform);
    
    output.dissolve = input.dissolve;
	
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

float4 PS(DissolveLightPixelInput input) : SV_TARGET
{
    Material material = GetMaterial(input);
	
    float4 color = 0;
    float4 emissive = mEmissive;
    
    float4 dissolve = DissolveMap.Sample(samp, input.uv);
    
    [flatten]
    if(dissolve.r > input.dissolve + 0.1f)
        color.w = 0.0f;
    else if(dissolve.r > input.dissolve)
    {
        float gradient = dissolve.r - input.dissolve;
        
        color.r = 0.5f - gradient * 5.0f;
        color.g = 0.25f - gradient * 2.5f;
        color.b = 0.0f;
        emissive.r = 0.5f;
        emissive.g = 0.0f;
        emissive.b = 0.0f;
        emissive.w = 0.2f;
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
                color += CalcDirectional(material, lights[i]);
            else if (lights[i].type == 1)
                color += CalcPoint(material, lights[i]);
            else if (lights[i].type == 2)
                color += CalcSpot(material, lights[i]);
            else if (lights[i].type == 3)
                color += CalcCapsule(material, lights[i]);
        }
    }
    
    float4 ambient = CalcAmbient(material);
	
    return color + ambient + emissive;
}