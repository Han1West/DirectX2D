#pragma once
#include <Engine/CScript.h>

class CSound;

class CPhoneScript :
    public CScript
{
private:
    Ptr<CSound> m_RingSound;
    float   m_AccTime;

    bool    m_bRing;
    bool    m_bSound;
    bool    m_ActiveScript;

    int m_ScriptCount;
    int m_SoundIdx;

public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

public:
    void PrintScript();

    void SetRing(bool _Ring) { m_bRing = _Ring; }
    bool IsRing() { return m_bRing; }

    void SetScript(bool _Script) { m_ActiveScript = _Script; }
    bool IsActiveScript() { return m_ActiveScript; }

public:
    CLONE(CPhoneScript);
    CPhoneScript();
    ~CPhoneScript();
};

