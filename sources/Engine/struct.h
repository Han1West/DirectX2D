#pragma once


struct Vtx
{
	Vec3	vPos;
	Vec2	vUV;
	Vec4	vColor;
};


struct tDebugShapeInfo
{
	DEBUG_SHAPE Shape;
	wstring		Name;

	Vec3		WorldPos;
	Vec3		Scale;	
	Vec3		Rotation;
	Matrix		matWorld;
	
	Vec4		Color;
	float		Duration;	// DebugShape 유지 시간
	float		Time;		// 현재 진행 시간

	bool		DepthTest;	// 깊이검사를 수행
};

struct tTask
{
	TASK_TYPE	Type;
	DWORD_PTR	Param0;
	DWORD_PTR	Param1;
	DWORD_PTR	Param2;
};

struct tLight2DInfo
{
	Vec3		vColor;		// 광원 색상	
	Vec3		vWorldPos;	// 광원 위치
	Vec3		vDir;		// 광원 진행 방향
	int			Type;		// 광원 타입(LIGHT_TYPE)
	float		Radius;		// 광원 범위반경
	float		Angle;		// 광원 범위각도	
};

// Pixel
struct tPixel
{
	BYTE b;
	BYTE g;
	BYTE r;
	BYTE a;
};


// Particle
struct tParticle
{
	Vec4	vColor;
	Vec3	vLocalPos;
	Vec3	vWorldPos;
	Vec3	vWorldInitScale;
	Vec3	vWorldCurrentScale;
	Vec3	vWorldRotation;

	Vec3	vForce;				// Particle 에 누적된 힘
	Vec3	vVelocity;

	float	NoiseForceAccTime;	// Noise Force 텀에 도달하는 누적 시간
	Vec3	NoiseForceDir;		// Noise Force 의 방향

	float	Mass;
	float	Life;
	float	Age;
	float	NormalizedAge;
	int		Active;

	Vec2	Padding;
};

// Particle Module
struct tParticleModule
{
	// Spawn
	UINT	SpawnRate;				// 초당 파티클 생성개수 (Spawn Per Second)
	Vec4	vSpawnColor;			// 생성 시점 색상
	Vec4	vSpawnMinScale;			// 생성 시 최소 크기
	Vec4	vSpawnMaxScale;			// 생성 시 최대 크기

	float	MinLife;				// 최소 수명
	float	MaxLife;				// 최대 수명

	UINT	SpawnShape;				// 0 : Box,  1: Sphere
	Vec3	SpawnShapeScale;		// SpawnShapeScale.x == Radius

	UINT	BlockSpawnShape;		// 0 : Box,  1: Sphere
	Vec3	BlockSpawnShapeScale;	// SpawnShapeScale.x == Radius

	UINT	SpaceType;				// 0 : LocalSpace, 1 : WorldSpace

	// Spawn Burst
	UINT	SpawnBurstCount;		// 한번에 발생시키는 Particle 수
	UINT	SpawnBurstRepeat;
	float	SpawnBurstRepeatTime;

	// Add Velocity
	UINT	AddVelocityType;		// 0 : Random, 1 : FromCenter, 2 : ToCenter, 4 : Fixed 
	Vec3	AddVelocityFixedDir;
	float	AddMinSpeed;
	float	AddMaxSpeed;

	// Scale Module
	float	StartScale;
	float	EndScale;

	// Drag Module (감속 모듈)
	float	DestNormalizedAge;
	float	LimitSpeed;

	// Noise Force Module
	float	NoiseForceTerm;		// Noise Force 적용시키는 텀
	float	NoiseForceScale;	// Noise Force 크기

	// Render Module
	Vec3	EndColor;			// 최종 색상
	UINT	FadeOut;			// 0 : Off, 1 : Normalized Age
	float	FadeOutStartRatio;	// FadeOut 효과가 시작되는 Normalized Age 지점
	UINT	VelocityAlignment;	// 속도 정렬 0 : Off, 1 : On


	// Module On / Off
	int		Module[(UINT)PARTICLE_MODULE::END];
};


struct tTileGridCell
{
	Vec2 Position;
	Vec2 Size;
	Vec2 GridIndex;
	bool Walkable;
};

// ====================
// 상수 버퍼 연동 구조체
// ====================
struct TransInfo
{
	Matrix	matWorld;
	Matrix	matView;
	Matrix	matProj;

	Matrix	matWV;
	Matrix	matWVP;
};
extern TransInfo g_Trans;

struct MtrlConst
{
	int		iArr[4];
	float	fArr[4];
	Vec2	v2Arr[4];
	Vec4	v4Arr[4];
	Matrix	mat[2];
	int		bTex[TEX_END];
};

struct SpriteInfo
{
	Vec2 LeftTopUV;
	Vec2 SliceUV;
	Vec2 BackgroundUV;
	Vec2 OffsetUV;
	int  SpriteUse;
	int  padding[3];
};

struct GlobalData
{
	Vec2	RenderResolution;	// 렌더링 해상도

	float	DeltaTime;		// DT
	float   Time;			// 누적시간

	float	DT_Engine;		// DT
	float   Time_Engine;	// 누적시간

	int		Light2DCount;		// 2D 광원 개수
	int		Light3DCount;		// 3D 광원 개수
};
extern GlobalData g_Data;

struct ShakeData
{
	Vec2 ShakeOffset;
	float ShakeStrength;
	float padding;
};

struct WeaponData
{
	int bGround;
	float paading[3];
};

struct tTileDebug
{
	int bTileDebug;
	float padding[3];
};

struct TimeData
{
	float Time;
	float padding[3];
};

struct CursorData
{
	int bAimFix;
	float padding[3];
};

struct MainUIData
{
	int bStartSelected;
	int bExitSelected;
	float padding[2];
};