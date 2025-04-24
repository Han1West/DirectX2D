#pragma once
#include <Engine/CScript.h>

class CDoorScript :
    public CScript
{
private:
    bool m_IsOpening;
    bool m_IsPlayerOpen;
    bool m_IsHorizontal;

    Vec3 m_OriginalPos;
    Vec3 m_OtherObjPos;

    float m_Speed;
    float m_AccTime;

public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

public:
    bool IsPlayerOpening() { return m_IsPlayerOpen; }

private:
    void OpenDoor(Vec3 _OtherObjPos);
   

public:
    CLONE(CDoorScript);
    CDoorScript();
    ~CDoorScript();
};

