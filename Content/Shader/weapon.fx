#ifndef _WEAPON
#define _WEAPON

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

VS_OUT VS_Weapon(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    float4 ShakePosition = ApplyShake(output.vPosition, g_ShakeOffset, g_ShakeStrength);
    output.vPosition = ShakePosition;
    
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
    output.vUV = _in.vUV;
    
    // �ð��� ���� ��鸲�� ���
    if (g_IsGround)
    {        
        float time = g_Time; 
        float amplitude = 0.01f; 
        float frequency = 3.f; 
        float offset = amplitude * sin(frequency * time);
        output.vPosition.y += offset; 
    }
        
    return output;
}

// Std2D
float4 PS_Weapon(VS_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    
    // Flipbook �� ���� Sprite �� ����ؾ� �ϴ� ���
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
        }
    }
    
    // FlipbookPlayer �� ���ų� ������� Flipbook �� ���� ���
    else
    {
        if (g_btex_0)
            vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        else
            vColor = GetDebugColor(_in.vUV, 10);
    }
       
    if (vColor.a == 0.f)
        discard;
    
    // ����ó��
    float3 LightColor = float3(0.f, 0.f, 0.f);
    
    for (int i = 0; i < g_Light2DCount; ++i)
    {
        CalcLight2D(i, _in.vWorldPos, LightColor);
    }
    
    vColor.rgb *= LightColor;
    
    // ��¦�Ÿ��� ȿ��
    if (g_IsGround)
    {
        float time = g_Time * 3.0f; // �ð� ������ ����
        float pulse = abs(sin(time)); // 0 ~ 1 �� ����
        float intensity = lerp(0.5f, 2.0f, pulse); // �⺻ ���(1.0)���� �ִ� ���(2.0)�� ��ȭ
        vColor.rgb *= intensity; // ���� ��⸦ ����              
    }
   
    return vColor;
}

#endif