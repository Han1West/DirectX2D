#ifndef _CURSOR
#define _CURSOR

#include "value.fx"
#include "func.fx"

// Vertex Shader 
struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float3 vWorldPos : POSITION;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_Cursor(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
            
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
    output.vUV = _in.vUV;
        
    return output;
}



float4 PS_Cursor(VS_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    if (g_SpriteUse)
    {
        //float2 vSpriteUV = g_LeftTopUV + (_in.vUV * g_SliceUV);
        
        float2 vBackroundLeftTop = g_LeftTopUV + (g_SliceUV / 2.f) - (g_BackgroundUV / 2.f);
        float2 vSpriteUV = vBackroundLeftTop + (_in.vUV * g_BackgroundUV) - g_OffsetUV;
        
        if (vSpriteUV.x < g_LeftTopUV.x || g_LeftTopUV.x + g_SliceUV.x < vSpriteUV.x
            || vSpriteUV.y < g_LeftTopUV.y || g_LeftTopUV.y + g_SliceUV.y < vSpriteUV.y)
        {
            //vColor = float4(1.f, 1.f, 0.f, 1.f);
            discard;
        }
        else
        {
            vColor = g_Atlas.Sample(g_sam_1, vSpriteUV);
            if (vColor.a > 0.f && g_IsAimFixed)
            {
                float3 RedColor = float3(0.8f, 0.1f, 0.1f);
                vColor.rgb *= RedColor;
            }

        }
    }
    else
    {
        if (g_btex_0)
            vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        else
            vColor = GetDebugColor(_in.vUV, 10);
    }
    
     // ±¤¿øÃ³¸®
    float3 LightColor = float3(0.f, 0.f, 0.f);
    
    for (int i = 0; i < g_Light2DCount; ++i)
    {
        CalcLight2D(i, _in.vWorldPos, LightColor);
    }
    
    vColor.rgb *= LightColor;
    
    if(vColor.a == 0.f)
        discard;
   
    
    return vColor;
}

#endif