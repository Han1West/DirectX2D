#pragma once
#include <Engine/CScript.h>

class CSound;

class CMainMenuScript :
    public CScript
{
private:
    Ptr<CSound> m_BGM;
    
    int m_CurIdx;


public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

public:
    int GetIdx() { return m_CurIdx; }

public:
    CLONE(CMainMenuScript);
    CMainMenuScript();
    ~CMainMenuScript();
};

