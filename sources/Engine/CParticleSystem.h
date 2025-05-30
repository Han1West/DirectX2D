#pragma once
#include "CRenderComponent.h"

#include "Ptr.h"
#include "CParticleTickCS.h"

class CStructuredBuffer;

struct tSpawnCount
{
    int SpawnCount;
    int Padding[3];
};

class CParticleSystem :
    public CRenderComponent
{
private:    
    Ptr<CParticleTickCS>    m_TickCS;
    CStructuredBuffer*      m_ParticleBuffer;       // 모든 파티클 정보
    CStructuredBuffer*      m_SpawnCountBuffer;     // 파티클 활성화 숫자 전달용 버퍼
    CStructuredBuffer*      m_ModuleBuffer;         // Module Data Buffer

    Ptr<CTexture>           m_ParticleTex;
    float                   m_Time;                 // 누적시간    
    float                   m_BurstTime;            // SpawnBurst 체크용 변수

    int                     m_MaxParticleCount;      // 파티클 최대 개수

    tParticleModule         m_Module;               // 파티클의 기능 정의

public:
    void SetParticleTexture(Ptr<CTexture> _Tex) { m_ParticleTex = _Tex; }
    tParticleModule GetModule() { return m_Module; }
    void SetModule(tParticleModule _Module) { m_Module = _Module; }

private:
    void CaculateSpawnCount();

public:
    virtual void FinalTick() override;
    virtual void Render() override;
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _FILE) override;

public:
    CLONE(CParticleSystem);
    CParticleSystem();
    CParticleSystem(const CParticleSystem& _Origin);
    ~CParticleSystem();
};

