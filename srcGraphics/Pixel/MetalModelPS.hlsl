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
    float4 g_diffuseColor;
    float4 g_ambientColor;
    float4 g_specularColor;
    float3 g_lightDirection;
    float dummy;
    
    float g_specularPower;
    float3 g_cameraPos;
};

Texture2D diffuseTexture : register(t0);
SamplerState diffuseSampler : register(s0);

float4 main(PS_INPUT input) : SV_TARGET0
{
    float4 color = diffuseTexture.Sample(diffuseSampler, input.uv);
    if (color.a < 0.01f)
    {
        discard;
    }
   
    float3 norm = normalize(input.normal);
    float3 lightDir = normalize(g_lightDirection);
    
    // ディフューズ
    float difDot = saturate(dot(norm, -lightDir));
    float3 diffuse = color.rgb * difDot * g_diffuseColor.rgb;
    
    // スペキュラ
    float3 toEye = normalize(g_cameraPos - input.worldPos);
    float3 reflectDir = normalize(lightDir + (norm * 2.0f * difDot));
    float refDot = saturate(dot(reflectDir, toEye));
    float refDotEx = pow(refDot, g_specularPower);
    float3 specular = refDotEx * g_specularColor.rgb * color.a;
    
    float3 rgb = diffuse + specular + g_ambientColor.rgb;
    
    return float4(rgb, color.a);
}