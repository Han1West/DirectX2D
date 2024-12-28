#ifndef _SHADER
#define _SHADER

cbuffer Transform : register(b0)
{    
    row_major matrix g_matWorld;
    row_major matrix g_matView;
    row_major matrix g_matProj;
};

Texture2D g_tex : register(t0);

// Vertex Shader 
struct VS_IN
{
    // semantic
    float3 vPos : POSITION;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_Test(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
        
    float3 vLocalPos = _in.vPos;
        
    // Local -> World
    float4 vWorldPos = mul(float4(vLocalPos, 1.f), g_matWorld);
    
    // World -> View
    float4 vViewPos = mul(vWorldPos, g_matView);
    
    // View -> Projection
    float4 vProjPos = mul(vViewPos, g_matProj);
        
    output.vPosition = vProjPos;    
    output.vColor = _in.vColor;
    output.vUV = _in.vUV;
        
    return output;
}

SamplerState g_sam_0 : register(s0);

// Pixel Shader
float4 PS_Test(VS_OUT _in) : SV_Target
{    
    float4 vColor = g_tex.Sample(g_sam_0, _in.vUV);
    return vColor;
}

#endif
