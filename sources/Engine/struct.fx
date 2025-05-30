#ifndef _STRUCT
#define _STRUCT

struct tLight2DInfo
{
    float3  vColor; // 광원 색상	
    float3  vWorldPos; // 광원 위치
    float3  vDir; // 광원 진행 방향
    int     Type; // 광원 타입(LIGHT_TYPE)
    float   Radius; // 광원 범위반경
    float   Angle; // 광원 범위각도	  
};


// Particle
struct tParticle
{
    float4  vColor;
    float3  vLocalPos;
    float3  vWorldPos;
    float3  vWorldInitScale;
    float3  vWorldCurrentScale;
    float3  vWorldRotation;
    
    float3  vForce;
    float3  vVelocity;
        
    float   NoiseForceAccTime; // Noise Force 텀에 도달하는 누적 시간
    float3  NoiseForceDir; // 적용된 Noise Forec 의 방향
            
    float   Mass;
    float   Life;
    float   Age;
    float   NormalizedAge;
    int     Active;
    
    float2  padding;
};


struct tParticleModule
{
    // Spawn
    uint    SpawnRate; // 초당 파티클 생성 개수
    float4  vSpawnColor; // 생성 시점 색상
    float4  vSpawnMinScale; // 생성 시 최소 크기
    float4  vSpawnMaxScale; // 생성 시 최대 크기

    float   MinLife;            // 최소 수명
    float   MaxLife;            // 최대 수명

    uint    SpawnShape;         // 0 : Box,  1: Sphere
    float3  SpawnShapeScale;    // SpawnShapeScale.x == Radius

    uint    BlockSpawnShape;    // 0 : Box,  1: Sphere
    float3  BlockSpawnShapeScale; // SpawnShapeScale.x == Radius    
    
    uint    SpaceType;          // 0 : LocalSpcae, 1 : WorldSpace
    
    // Spawn Burst
    uint    SpawnBurstCount;    // 한번에 발생시키는 Particle 수
    uint    SpawnBurstRepeat;
    float   SpawnBurstRepeatTime;
    
    // Add Velocity
    uint    AddVelocityType;    // 0 : Random, 1 : FromCenter, 2 : ToCenter, 4 : Fixed 
    float3  AddVelocityFixedDir;
    float   AddMinSpeed;
    float   AddMaxSpeed;
    
    // Scale Module
    float   StartScale;
    float   EndScale;
    
    // Drag Module
    float   DestNormalizedAge;
    float   LimitSpeed;    
    
    // Noise Force Module
    float   NoiseForceTerm;     // Noise Force 적용시키는 텀
    float   NoiseForceScale;    // Noise Force 의 크기
       
    // Render Module
    float3  EndColor;           // 최종 색상
    uint    FadeOut;            // 0 : Off, 1 : Normalized Age
    float   StartRatio;         // FadeOut 효과가 시작되는 Normalized Age 지점
    uint    VelocityAlignment;  // 속도 정렬 0 : Off, 1 : On
    
	// Module On / Off
    int     Module[7];
};

#endif