#ifndef _FUNC
#define _FUNC

#include "value.fx"
#include "struct.fx"

float4 GetDebugColor(float2 _UV, int _Level)
{
    float2 vTexPos = _UV * _Level;
    
    int Col = floor(vTexPos.x);
    int Row = floor(vTexPos.y);
        
    // 홀수
    if (Col % 2)
    {
        if (Row % 2)
            return float4(1.f, 0.f, 1.f, 1.f);
        else
            return float4(0.f, 0.f, 0.f, 1.f);
    }           
    // 짝수
    else
    {
        if (Row % 2)
            return float4(0.f, 0.f, 0.f, 1.f);
        else
            return float4(1.f, 0.f, 1.f, 1.f);
    }
}

void CalcLight2D(int _LightIdx, float3 _WorldPos, inout float3 _LightColor)
{   
    float DistRatio = 1.f;
    
    // 광원처리
    if (g_Light2DInfo[_LightIdx].Type == 0) // DirLight
    {
        _LightColor += g_Light2DInfo[_LightIdx].vColor;
    }
    else if (g_Light2DInfo[_LightIdx].Type == 1) // PointLight
    {
        float Dist = distance(_WorldPos.xy, g_Light2DInfo[_LightIdx].vWorldPos.xy);
        if (Dist <= g_Light2DInfo[_LightIdx].Radius)
        {
            DistRatio = saturate(1.f - (Dist / g_Light2DInfo[_LightIdx].Radius));
            //DistRatio = saturate(cos(saturate((Dist / g_Light2DInfo[_LightIdx].Radius)) * (PI / 2.f)));
            
            _LightColor += g_Light2DInfo[_LightIdx].vColor * DistRatio;
        }
    }
    
    else // SpotLight
    {
        
    }
}


float3 GetRandom(in Texture2D _NoiseTexture, uint _ID, uint _maxId)
{
    float2 vUV = (float2) 0.f;
    
    vUV.x = ((float) _ID / (float) (_maxId - 1)) + g_Time_Engine * 0.01f;
    vUV.y = sin(vUV.x * 20 * PI) * 0.5f + g_Time_Engine * 0.1f;
    float3 vRandom = _NoiseTexture.SampleLevel(g_sam_0, vUV, 0).xyz;
    
    return vRandom;
}

float4 ApplyShake(float4 _pos, float2 _ShakeOffset, float _ShakeStringth)
{
    _pos.xy += _ShakeOffset * _ShakeStringth;
    
    return _pos;
    
}

float4 LerpColor(float4 colorA, float4 colorB, float t)
{
    return lerp(colorA, colorB, t);
}

float2 RotateUV(float2 uv, float angle, float2 center)
{
    // UV 좌표를 중심으로 이동
    uv -= center;

    // 회전 변환
    float c = cos(angle);
    float s = sin(angle);
    float2 rotatedUV = float2(
        uv.x * c - uv.y * s,
        uv.x * s + uv.y * c
    );

    // 다시 중심으로 복귀
    rotatedUV += center;
    return rotatedUV;
}

#endif