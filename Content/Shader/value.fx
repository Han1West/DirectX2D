#ifndef _VALUE
#define _VALUE

#include "struct.fx"

// 4096
cbuffer Transform : register(b0)
{
    row_major matrix g_matWorld;
    row_major matrix g_matView;
    row_major matrix g_matProj;
    
    row_major matrix g_matWV;
    row_major matrix g_matWVP;
};

cbuffer Material : register(b1)
{
    int              g_int_0;
    int              g_int_1;
    int              g_int_2;
    int              g_int_3;   
    
    float            g_float_0;
    float            g_float_1;
    float            g_float_2;
    float            g_float_3;    
    
    float2           g_vec2_0;
    float2           g_vec2_1;
    float2           g_vec2_2;
    float2           g_vec2_3;
    
    float4           g_vec4_0;
    float4           g_vec4_1;
    float4           g_vec4_2;
    float4           g_vec4_3;  
    
    row_major Matrix g_mat_0;
    row_major Matrix g_mat_1;    
    
    int              g_btex_0;
    int              g_btex_1;
    int              g_btex_2;
    int              g_btex_3;
    int              g_btex_4;
    int              g_btex_5;
    int              g_btexarr_0;
    int              g_btexarr_1;
    int              g_btexarr_2;
    int              g_btexarr_3;
    int              g_btexcube_0;
    int              g_btexcube_1;
}

cbuffer SPRITE : register(b2)
{
    float2  g_LeftTopUV;
    float2  g_SliceUV;
    float2  g_BackgroundUV;
    float2  g_OffsetUV;    
    
    int     g_SpriteUse;
    int3    g_SpritePadding;
}

cbuffer GLOBAL : register(b3)
{
    float2  g_RenderResolution;   // 렌더링 해상도
    float   g_DeltaTime;          // DT
    float   g_Time;               // 누적시간
    float   g_DT_Engine;          // DT
    float   g_Time_Engine;        // 누적시간
    int     g_Light2DCount;       // 2D 광원 개수
    int     g_Light3DCount;       // 3D 광원 개수
}

cbuffer ShakeBuffer : register(b4)
{
    float2 g_ShakeOffset;
    float g_ShakeStrength;
    float padding_Shake;
}

cbuffer WeaponBuffer : register(b5)
{
    int g_IsGround;
    float padding_Weapon[3];
}

cbuffer TileDebugBuffer : register(b6)
{
    int g_IsDebug;
    float padding_Tile[3];
}

cbuffer TimeDataBuffer : register(b7)
{
    float g_NewTime;
    float3 paading_Time[3];
}

cbuffer CursorDataBuffer : register(b8)
{
    int g_IsAimFixed;
    float3 paading_Cursor[3];
}

cbuffer MainUIDataBuffer : register(b9)
{
    int g_StartSelected;
    int g_ExitSelected;
    float3 paading_Main[2];
}

Texture2D       g_tex_0 : register(t0);
Texture2D       g_tex_1 : register(t1);
Texture2D       g_tex_2 : register(t2);
Texture2D       g_tex_3 : register(t3);
Texture2D       g_tex_4 : register(t4);
Texture2D       g_tex_5 : register(t5);

Texture2DArray  g_texarr_0 : register(t6);
Texture2DArray  g_texarr_1 : register(t7);
Texture2DArray  g_texarr_2 : register(t8);
Texture2DArray  g_texarr_3 : register(t9);

TextureCube     g_texcube_0 : register(t10);
TextureCube     g_texcube_1 : register(t11);

Texture2D       g_Atlas : register(t12);    // Sprite 전용 텍스쳐 레지스터

StructuredBuffer<tLight2DInfo>   g_Light2DInfo : register(t13); // 2D 광원 정보
//StructuredBuffer<tLight3DInfo> g_Light3DInfo : register(t14); // 2D 광원 정보
// StructuredBuffer<tBone>       g_Light3DInfo : register(t15); // 3D Animation Skinning Matrix

SamplerState    g_sam_0 : register(s0); // 이방성 필터링
SamplerState    g_sam_1 : register(s1); // MIN_MAG_POINT 필터링


#define PI  3.1415926535f

// 1. VertexBuffer
// 2. IndexBuffer
// 3. ConstBuffer(크기에 제약, 가변성)
// 4. StructuredBuffer( 구조화 버퍼 )

#endif