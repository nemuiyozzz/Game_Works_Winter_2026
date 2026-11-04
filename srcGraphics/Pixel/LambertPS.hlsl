struct PS_INPUT
{
    float4 svPos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 vwPos : TEXCOORD1;
    float3 normal : TEXCOORD2;
    float4 diffuse : COLOR0;
    float3 lightDir : TEXCOORD3;
    float3 lightAtPos : TEXCOORD4;
    float3 worldPos : TEXCOORD5;
};

cbuffer cbParam : register(b4)
{
    float3 g_lightDirection;
    float dummy;
};

Texture2D diffuseTexture : register(t0);
SamplerState diffuseSampler : register(s0);

float4 main(PS_INPUT input) : SV_TARGET0
{
    //float4 color = diffuseTexture.Sample(diffuseSampler, input.uv);
    //if (color.a < 0.01f)
    //{
    //    discard;
    //}
    
    //float3 norm = normalize(input.normal);
    //float lightDot = saturate(dot(norm, -g_lightDirection));
    
    //float3 rgb = color.rgb * lightDot;
    //rgb += color.rgb * float3(0.2f, 0.2f, 0.2f);
    
    //return float4(rgb, color.a);
    
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}