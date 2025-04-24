#pragma once
#include <Engine/CScript.h>

class CCarScript :
    public CScript
{
private:
    float   m_AccTime;

    bool    m_PlayerRide;
    bool    m_bLevelClear;
    bool    m_bCloseSound;
    bool    m_bEngineSound;

public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

public:
    bool GetRide() {return m_PlayerRide; }

    void ChangeStage();
public:
    CLONE(CCarScript);
    CCarScript();
    ~CCarScript();
};

