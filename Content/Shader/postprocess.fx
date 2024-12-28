#ifndef _POSTPROCESS
#define _POSTPROCESS

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
    float2 vUV : TEXCOORD;
};

// Mesh : RectMesh

VS_OUT VS_Post(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
            
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    output.vUV = _in.vUV;
        
    return output;
}

float4 PS_Post(VS_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
            
    vColor = g_tex_0.Sample(g_sam_0, _in.vUV);    
    float Aver = (vColor.r + vColor.g + vColor.b) / 3.f;    
    vColor.rgb = Aver;
        
    return vColor;
}



// =================
// Distortion Shader
// =================
VS_OUT VS_Distortion(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
            
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
        
    return output;
}

// 1. 픽셀쉐이더의 픽셀 좌표
// 2. 렌더 타겟 해상도

float4 PS_Distortion(VS_OUT _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    // SV_Position -> Pixelshader (픽셀좌표)
    float2 vScreenUV = _in.vPosition.xy / g_RenderResolution;    
    
    // NoiseTexture 가 있으면
    if(g_btex_1)
    {
        float4 vNoise = g_tex_1.Sample(g_sam_0, vScreenUV + 0.05f * g_Time);
        vNoise -= 0.5f;    
        vNoise *= 0.05f;        
        vScreenUV += vNoise.xy;
    }
    
    vColor = g_tex_0.Sample(g_sam_0, vScreenUV);
    
    return vColor;
}



struct VS_VortexOut
{
    float4 vPosition : SV_Position;
    float2 vObjectUV : TEXCOORD;
};


VS_VortexOut VS_Vortex(VS_IN _in)
{
    VS_VortexOut output = (VS_VortexOut) 0.f;
            
    output.vPosition = float4(_in.vPos * 2.f, 1.f);
        
    float4 vProjPos = mul(float4(0.f, 0.f, 0.f, 1.f), g_matWVP);
    vProjPos.xyz = vProjPos.xyz / vProjPos.w;
    
    output.vObjectUV.x = (vProjPos.x + 1.f) / 2.f;
    output.vObjectUV.y = 1.f - ((vProjPos.y + 1.f) / 2.f);
        
    return output;
}


float4 PS_Vortex(VS_VortexOut _in) : SV_Target
{
    float4 vColor = (float4) 0.f;
    
    float effectRadius = 0.1f;
    float effectAngle = 1.f * PI;
    
    float2 center = _in.vObjectUV;
    
    float2 uv = (_in.vPosition.xy / g_RenderResolution.xy) - center;
    
    float len = length(uv * float2(g_RenderResolution.x / g_RenderResolution.y, 1.));
    float angle = atan2(uv.y, uv.x) + effectAngle * smoothstep(effectRadius, 0., len);
    float radius = length(uv);

    vColor = g_tex_0.Sample(g_sam_0, float2(radius * cos(angle), radius * sin(angle)) + center);
    
    return vColor;
}




// 화면 노이즈 효과

struct VS_NoiseOut
{
    float4 vPosition : SV_Position;
    float2 vObjectUV : TEXCOORD;
};


VS_NoiseOut VS_Noise(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
            
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    output.vUV = _in.vUV;
        
    return output;
}


float4 PS_Noise(VS_NoiseOut _in) : SV_Target
{
    float4 vColor = (float4) 0.f;

    // UV 좌표 계산
    float2 vScreenUV = _in.vPosition.xy / g_RenderResolution;

    // 노이즈 텍스처가 있을 경우
    if (g_btex_1)
    {
        // 노이즈 텍스처 샘플링
        float4 vNoise = g_tex_1.Sample(g_sam_0, float2(vScreenUV.x, vScreenUV.y + g_Time * 0.1f));
        
        // 노이즈 값의 밝기를 계산하여 가로 줄무늬 효과 생성
        float stripeIntensity = abs(sin(vScreenUV.y * 50.0f)); // 주기적인 가로줄
        stripeIntensity *= vNoise.r * 0.4f; // 노이즈 텍스처와 결합

        // 기본 색상에 노이즈 효과 적용
        vColor = g_tex_0.Sample(g_sam_0, _in.vObjectUV); // 기본 텍스처 샘플링
        vColor.rgb += stripeIntensity * 0.2f; // 밝기 조정
    }
    else
    {
        // 노이즈 텍스처가 없을 경우 기본 색상 샘플링
        vColor = g_tex_0.Sample(g_sam_0, vScreenUV);
    }
    

    // 화면을 어둡게 하면서 저녁 같은 분위기 추가
    float fogAmount = 0.25f; // 안개의 강도 (0.0 ~ 1.0)
    float3 eveningTint = float3(0.2f, 0.2f, 0.3f); // 어두운 파란빛
    vColor.rgb = lerp(vColor.rgb, eveningTint, fogAmount); // 원래 색상과 혼합
    
    // 색상을 조금 더 어둡게
    vColor.rgb *= 0.85f; // 전체 밝기 감소

    return vColor;
}


// Fade Out-in
struct VS_FadeOut
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    float3 vWorldPos : TEXCOORD1;
};


VS_FadeOut VS_Fade(VS_IN _in)
{
    VS_FadeOut output = (VS_FadeOut) 0.f;
            
    output.vPosition = float4(_in.vPos.xy * 2.f , 0.f, 1.f);
    output.vUV = _in.vUV;
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    return output;
}


float4 PS_Fade(VS_FadeOut _in) : SV_Target
{   
    // 원본 화면 색상
    float4 originalColor = float4(0.f, 0.f, 0.f, 0.f);
    if (g_SpriteUse)
    {
        float2 vBackroundLeftTop = g_LeftTopUV + (g_SliceUV / 2.f) - (g_BackgroundUV / 2.f);
        float2 vSpriteUV = vBackroundLeftTop + (_in.vUV * g_BackgroundUV) - g_OffsetUV;
        
        if (vSpriteUV.x < g_LeftTopUV.x || g_LeftTopUV.x + g_SliceUV.x < vSpriteUV.x
            || vSpriteUV.y < g_LeftTopUV.y || g_LeftTopUV.y + g_SliceUV.y < vSpriteUV.y)
        {
            discard;
        }
        else
        {
            originalColor = g_Atlas.Sample(g_sam_1, vSpriteUV);
        }
    }
    else
    {
        // 첫번째 텍스쳐는 물체의 색상
        if (g_btex_0)
            originalColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        else
            originalColor = GetDebugColor(_in.vUV, 10);
    }

    
    // 화면 어둡게 처리
    float transition = saturate(g_NewTime / 1.f); // 페이드 진행 상태
    float4 darkColor = float4(0.0f, 0.0f, 0.0f, 1.0f); // 검은색

  
    return lerp(darkColor, originalColor, transition);
}


// Highlight
struct VS_HighlightOut
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    float3 vWorldPos : TEXCOORD1;
};


VS_HighlightOut VS_Highlight(VS_IN _in)
{
    VS_HighlightOut output = (VS_HighlightOut) 0.f;
            
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    output.vUV = _in.vUV;
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    return output;
}


float4 PS_Highlight(VS_HighlightOut _in) : SV_Target
{
    // 원본 화면 색상
    float4 originalColor = float4(0.f, 0.f, 0.f, 0.f);
    if (g_SpriteUse)
    {
        float2 vBackroundLeftTop = g_LeftTopUV + (g_SliceUV / 2.f) - (g_BackgroundUV / 2.f);
        float2 vSpriteUV = vBackroundLeftTop + (_in.vUV * g_BackgroundUV) - g_OffsetUV;
        
        if (vSpriteUV.x < g_LeftTopUV.x || g_LeftTopUV.x + g_SliceUV.x < vSpriteUV.x
            || vSpriteUV.y < g_LeftTopUV.y || g_LeftTopUV.y + g_SliceUV.y < vSpriteUV.y)
        {
            discard;
        }
        else
        {
            originalColor = g_Atlas.Sample(g_sam_1, vSpriteUV);
        }
    }
    else
    {
        // 첫번째 텍스쳐는 물체의 색상
        if (g_btex_0)
            originalColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        else
            originalColor = GetDebugColor(_in.vUV, 10);
    }
    
    float4 darkColor = float4(0.0f, 0.0f, 0.0f, 1.0f); // 검은색

    float blackBarHeight = 0.1f; // 검은색 영역의 높이 비율 (0.1 = 10%)
    if (_in.vUV.y < blackBarHeight || _in.vUV.y > (1.0f - blackBarHeight))
    {
        return darkColor; // 위아래 검은색으로 설정
    }

    return originalColor; // 가운데는 원래 색상 유지
}


// Highlight Ingame
struct VS_HighlightIngameOut
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    float3 vWorldPos : TEXCOORD1;
};


VS_HighlightIngameOut VS_HighlightIngame(VS_IN _in)
{
    VS_HighlightIngameOut output = (VS_HighlightIngameOut) 0.f;
            
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.f, 1.f);
    output.vUV = _in.vUV;
    output.vWorldPos = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    return output;
}


float4 PS_HighlightIngame(VS_HighlightIngameOut _in) : SV_Target
{
    // 원본 화면 색상
    float4 originalColor = float4(0.f, 0.f, 0.f, 0.f);
    if (g_SpriteUse)
    {
        float2 vBackroundLeftTop = g_LeftTopUV + (g_SliceUV / 2.f) - (g_BackgroundUV / 2.f);
        float2 vSpriteUV = vBackroundLeftTop + (_in.vUV * g_BackgroundUV) - g_OffsetUV;
        
        if (vSpriteUV.x < g_LeftTopUV.x || g_LeftTopUV.x + g_SliceUV.x < vSpriteUV.x
            || vSpriteUV.y < g_LeftTopUV.y || g_LeftTopUV.y + g_SliceUV.y < vSpriteUV.y)
        {
            discard;
        }
        else
        {
            originalColor = g_Atlas.Sample(g_sam_1, vSpriteUV);
        }
    }
    else
    {
        // 첫번째 텍스쳐는 물체의 색상
        if (g_btex_0)
            originalColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        else
            originalColor = GetDebugColor(_in.vUV, 10);
    }
    
    float4 darkColor = float4(0.0f, 0.0f, 0.0f, 1.0f); // 검은색

    float blackBarHeight = 0.3f; // 검은색 영역의 높이 비율 (0.1 = 10%)
    if (_in.vUV.y < blackBarHeight || _in.vUV.y > (1.0f - blackBarHeight))
    {
        return darkColor; // 위아래 검은색으로 설정
    }

    return originalColor; // 가운데는 원래 색상 유지
}
#endif