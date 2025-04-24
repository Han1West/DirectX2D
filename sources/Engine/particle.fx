#ifndef _PARTICLE
#define _PARTICLE

#include "value.fx"
#include "func.fx"

StructuredBuffer<tParticle> ParticleBuffer : register(t16);
StructuredBuffer<tParticleModule> ModuleBuffer : register(t17);
#define Module      ModuleBuffer[0]

struct VS_IN
{
    float3  vPos    : POSITION;    
    uint    ID      : SV_InstanceID;
};

struct VS_OUT
{
    float3  vLocalPos   : POSITION;
    uint    ID          : FOG;
};

VS_OUT VS_Particle(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
       
    output.vLocalPos = _in.vPos;
    output.ID = (float)_in.ID;
    
    return output;
}


struct GS_OUT
{
    float4  vPosition   : SV_Position;
    float2  vUV         : TEXCOORD;
    uint    InstID      : FOG;
};

// Geometry Shader
// 1. 비활성화 된 파티클을 Rasterizer 로 보내지 않는다.
// 2. Billboard 효과

[maxvertexcount(32)]
void GS_Particle(point VS_OUT _in[1], inout TriangleStream<GS_OUT> _OutStream)
{
    GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    GS_OUT output_cross[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    
    tParticle Particle = ParticleBuffer[_in[0].ID];
    
    if (false == Particle.Active)
    {
        return;
    }
    
    //  0 -- 1
    //  | \  |
    //  3 -- 2
    float3 scale = Particle.vWorldCurrentScale;
    float3 vViewPos = mul(float4(Particle.vWorldPos, 1.f), g_matView).xyz;
        
    output[0].vPosition = float4(-scale.x / 2.f, scale.y / 2.f, 0.f, 0.f);
    output[1].vPosition = float4(scale.x / 2.f, scale.y / 2.f, 0.f, 0.f);
    output[2].vPosition = float4(scale.x / 2.f, -scale.y / 2.f, 0.f, 0.f);
    output[3].vPosition = float4(-scale.x / 2.f, -scale.y / 2.f, 0.f, 0.f);
    
    
    if (Module.VelocityAlignment)
    {
        output_cross[0].vPosition = float4(-scale.x * 0.5f, 0.f, scale.y * 0.5f, 0.f);
        output_cross[1].vPosition = float4(scale.x * 0.5f, 0.f, scale.y * 0.5f, 0.f);
        output_cross[2].vPosition = float4(scale.x * 0.5f, 0.f, -scale.y * 0.5f, 0.f);
        output_cross[3].vPosition = float4(-scale.x * 0.5f, 0.f, -scale.y * 0.5f, 0.f);
                
        float3 vR = normalize(mul(float4(Particle.vVelocity, 0.f), g_matView).xyz);
        float3 vF = normalize(cross(vR, float3(0.f, 1.f, 0.f)));
        float3 vU = normalize(cross(vF, vR));

        float3x3 vRot =
        {
            vR,
            vU,
            vF,
        };
        
        for (int i = 0; i < 4; ++i)
        {
            output[i].vPosition.xyz = mul(output[i].vPosition.xyz, vRot);
            output_cross[i].vPosition.xyz = mul(output_cross[i].vPosition.xyz, vRot);
        }
    }
        
    for (int i = 0; i < 4; ++i)
    {
        output[i].vPosition.xyz += vViewPos;
        output[i].vPosition.w = 1.f;
        output[i].vPosition = mul(output[i].vPosition, g_matProj);
        output[i].InstID = _in[0].ID;
    }
    
    
    // 생성시킨 정점의 UV 세팅
    output[0].vUV = float2(0.f, 0.f);
    output[1].vUV = float2(1.f, 0.f);
    output[2].vUV = float2(1.f, 1.f);
    output[3].vUV = float2(0.f, 1.f);
    
    
    // Stream 출력
    _OutStream.Append(output[0]);
    _OutStream.Append(output[2]);
    _OutStream.Append(output[3]);
    _OutStream.RestartStrip();
    
    _OutStream.Append(output[0]);
    _OutStream.Append(output[1]);
    _OutStream.Append(output[2]);
    _OutStream.RestartStrip();
    
    
    if (Module.VelocityAlignment)
    {
        for (int i = 0; i < 4; ++i)
        {
            output_cross[i].vPosition.xyz += vViewPos;
            output_cross[i].vPosition.w = 1.f;
            output_cross[i].vPosition = mul(output_cross[i].vPosition, g_matProj);
            output_cross[i].InstID = _in[0].ID;
        }
    
        // 생성시킨 정점의 UV 세팅
        output_cross[0].vUV = float2(0.f, 0.f);
        output_cross[1].vUV = float2(1.f, 0.f);
        output_cross[2].vUV = float2(1.f, 1.f);
        output_cross[3].vUV = float2(0.f, 1.f);
           
        // OutStream 에 RectMesh 를 만들기 위해서 총 6개의 출력을 지정한다.
        _OutStream.Append(output_cross[0]);
        _OutStream.Append(output_cross[1]);
        _OutStream.Append(output_cross[2]);
        _OutStream.RestartStrip();
    
        _OutStream.Append(output_cross[0]);
        _OutStream.Append(output_cross[2]);
        _OutStream.Append(output_cross[3]);
        _OutStream.RestartStrip();
    }
}

float4 PS_Particle(GS_OUT _in) : SV_Target
{
    float4 vParticleColor = float4(1.f, 4.f, 2.f, 1.f);
   
    //if (g_SpriteUse)
    //{
    //    float2 vBackroundLeftTop = g_LeftTopUV + (g_SliceUV / 2.f) - (g_BackgroundUV / 2.f);
    //    float2 vSpriteUV = vBackroundLeftTop + (_in.vUV * g_BackgroundUV) - g_OffsetUV;
        
    //    if (vSpriteUV.x < g_LeftTopUV.x || g_LeftTopUV.x + g_SliceUV.x < vSpriteUV.x
    //        || vSpriteUV.y < g_LeftTopUV.y || g_LeftTopUV.y + g_SliceUV.y < vSpriteUV.y)
    //    {
    //        //vColor = float4(1.f, 1.f, 0.f, 1.f);
    //        discard;
    //    }
    //    else
    //    {
    //        vParticleColor = g_Atlas.Sample(g_sam_1, vSpriteUV);
    //    }
    //}

    if (g_btex_0)
    {
        float2 rotatedUV = RotateUV(_in.vUV, 45.f, float2(0.5f, 0.5f));
        vParticleColor = g_tex_0.Sample(g_sam_0, rotatedUV);
        vParticleColor *= ParticleBuffer[_in.InstID].vColor;
    }

    
        
    return vParticleColor;
}

#endif