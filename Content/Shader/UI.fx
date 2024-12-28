#ifndef _UI
#define _UI

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

VS_OUT VS_UI(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
            
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
    output.vUV = _in.vUV;
        
    return output;
}


float4 PS_UI(VS_OUT _in) : SV_Target
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
            //vColor = float4(0.5f, 0.5f, 0.5f, 1.f);
            discard;
        }
        else
        {
            vColor = g_Atlas.Sample(g_sam_1, vSpriteUV);
            if (vColor.a == 0)
                discard;
        }
    }
    else
    {
        if (g_btex_0)
        {
            vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
            if(vColor.a == 0)
                discard;
        }
        else
            vColor = GetDebugColor(_in.vUV, 10);
    }
    
     // 광원처리
    float3 LightColor = float3(0.f, 0.f, 0.f);
    
    for (int i = 0; i < g_Light2DCount; ++i)
    {
        CalcLight2D(i, _in.vWorldPos, LightColor);
    }
    
    vColor.rgb *= LightColor;
    
    // 시간 기반 밝기 변화 효과
    float timeFactor = abs(sin(g_Time * 2.0f)); // g_Time은 외부에서 전달되는 시간 변수
    vColor.rgb = lerp(vColor.rgb, float3(1.0f, 0.8f, 0.7f), timeFactor);

    
    return vColor;
}

float4 PS_MainBackground(VS_OUT _in) : SV_Target
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
            //vColor = float4(0.5f, 0.5f, 0.5f, 1.f);
            discard;
        }
        else
        {
            vColor = g_Atlas.Sample(g_sam_1, vSpriteUV);
            if (vColor.a == 0)
                discard;
        }
    }
    else
    {
        if (g_btex_0)
        {
            vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
            if (vColor.a == 0)
                discard;
        }
        else
            vColor = GetDebugColor(_in.vUV, 10);
    }
    
     // 광원처리
    float3 LightColor = float3(0.f, 0.f, 0.f);
    
    for (int i = 0; i < g_Light2DCount; ++i)
    {
        CalcLight2D(i, _in.vWorldPos, LightColor);
    }
    
    vColor.rgb *= LightColor;
    
    float4 greenColor = float4(0.1f, 1.0f, 0.1f, 1.0f);
    float4 purpleColor = float4(1.0f, 0.1f, 0.1f, 1.0f);
    float4 blueColor = float4(0.1f, 0.1f, 1.0f, 1.0f);

    // 색상 변화 주기 
    float cycleDuration = 6.0f;
    float t = fmod(g_Time, cycleDuration) / cycleDuration;

    // 색상 보간
    float4 currentColor;
    if (t < 1.0f / 3.0f)
    {
        currentColor = LerpColor(greenColor, purpleColor, t * 3.0f); // 초록색에서 보라색으로
    }
    else if (t < 2.0f / 3.0f)
    {
        currentColor = LerpColor(purpleColor, blueColor, (t - 1.0f / 3.0f) * 3.0f); // 보라색에서 파란색으로
    }
    else
    {
        currentColor = LerpColor(blueColor, greenColor, (t - 2.0f / 3.0f) * 3.0f); // 파란색에서 초록색으로
    }

    // 먹구름 낀 느낌을 주기 위해 회색조 추가
    //float3 grayScale = float3(0.2f, 0.2f, 0.2f);
    //currentColor.rgb = lerp(currentColor.rgb, grayScale, 0.5f);

    vColor = currentColor;
    
    return vColor;
}
#endif
