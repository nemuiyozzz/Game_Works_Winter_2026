struct VsBase
{
    matrix antiViewportMatrix;
    matrix projectionMatrix;
    float4x3 viewMatrix;
    float4x3 localWorldMatrix;
    float4 toonOutLineSize;
    float diffuseSource;
    float specularSource;
    float mulSpecularColor;
    float padding;
};
cbuffer cbD3D11_CONST_BUFFER_VS_BASE : register(b1)
{
    VsBase g_base;
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 norm : NORMAL;
    float4 diffuse : COLOR0;
    float4 specular : COLOR1;
    float4 uv0 : TEXCOORD0;
    float4 uv1 : TEXCOORD1;
    float3 tan : TANGENT0;
    float3 bin : BINORMAL0;
};

struct VS_OUTPUT
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

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    // 行列計算
    float4 localPos = float4(input.pos, 1.0f);
    float4 worldPos = float4(mul(localPos, g_base.localWorldMatrix), 1.0f);
    float4 viewPos = float4(mul(worldPos, g_base.viewMatrix), 1.0f);
    
    output.svPos = mul(viewPos, g_base.projectionMatrix);
    output.worldPos = worldPos.xyz;
    output.vwPos = viewPos.xyz;
    
    // 法線の回転
    output.normal = normalize(mul(input.norm, (float3x3) g_base.localWorldMatrix));
    
    output.uv = input.uv0.xy;
    output.diffuse = input.diffuse;
    output.lightDir = float3(0, 0, 0);
    output.lightAtPos = float3(0, 0, 0);
    
    return output;
}