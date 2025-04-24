#ifndef _TILEMAP
#define _TILEMAP

#include "value.fx"
#include "func.fx"

#define TileAtlas   g_tex_0
#define COL         g_int_0
#define ROW         g_int_1

struct tTileInfo
{
    float2 TileLT;
    float2 TileSlice;
};

StructuredBuffer<tTileInfo> g_Buffer : register(t16);

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

VS_OUT VS_TileMap(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
    output.vUV = _in.vUV * float2(COL, ROW);
    
    return output;
}


float4 PS_TileMap(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    int2 ColRow = floor(_in.vUV);
    int idx = ColRow.y * COL + ColRow.x;
        
    float2 vAtlasUV = g_Buffer[idx].TileLT + (frac(_in.vUV) * g_Buffer[idx].TileSlice);
    
    if(g_btex_0)
    {        

        if(vAtlasUV.x > 0.f)
            vOutColor = TileAtlas.Sample(g_sam_1, vAtlasUV);
        else
        {
            float4 greenColor = float4(0.0f, 1.0f, 0.0f, 1.0f); 
            float4 purpleColor = float4(1.0f, 0.0f, 1.0f, 1.0f); 
            float4 blueColor = float4(0.0f, 0.0f, 1.0f, 1.0f); 

            // ���� ��ȭ �ֱ� 
            float cycleDuration = 6.0f; 
            float t = fmod(g_Time, cycleDuration) / cycleDuration;

            // ���� ����
            float4 currentColor;
            if (t < 1.0f / 3.0f)
            {
                currentColor = LerpColor(greenColor, purpleColor, t * 3.0f); // �ʷϻ����� ���������
            }
            else if (t < 2.0f / 3.0f)
            {
                currentColor = LerpColor(purpleColor, blueColor, (t - 1.0f / 3.0f) * 3.0f); // ��������� �Ķ�������
            }
            else
            {
                currentColor = LerpColor(blueColor, greenColor, (t - 2.0f / 3.0f) * 3.0f); // �Ķ������� �ʷϻ�����
            }

            // �Ա��� �� ������ �ֱ� ���� ȸ���� �߰�
            float3 grayScale = float3(0.2f, 0.2f, 0.2f);
            currentColor.rgb = lerp(currentColor.rgb, grayScale, 0.5f);

            vOutColor = currentColor;
        }
            
    }
    else
    {
        vOutColor = GetDebugColor(_in.vUV, 10);
    }
    
 
    if (g_IsDebug)
    {
    // ���� Ÿ���� UV ��ǥ ���
        float2 localUV = frac(_in.vUV); // Ÿ�� �� ���� UV ��ǥ [0,1]
    
    // ��輱 �β� ����
        float borderThickness = 0.02f; // Ÿ�� ��輱�� �β�
    
    // ��輱 ����
        if (localUV.x < borderThickness || localUV.x > (1.0f - borderThickness) ||
        localUV.y < borderThickness || localUV.y > (1.0f - borderThickness))
        {
        // �ʷϻ� ��輱 ����
            vOutColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
        }
    }
    
    float3 LightColor = float3(0.f, 0.f, 0.f);    
    
    for (int i = 0; i < g_Light2DCount; ++i)
    {
        CalcLight2D(i, _in.vWorldPos, LightColor);
    }     
    
    vOutColor.rgb *= LightColor;    


    return vOutColor;
}

#endif