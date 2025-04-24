#pragma once
#include <Engine/CScript.h>

class CMeleeAttackScript :
    public CScript
{
private:
    float   m_LifeTime;

    WEAPON_TYPE m_CurWeapon;

public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

public:
    void SetWeapon(WEAPON_TYPE _Type) { m_CurWeapon = _Type; }

    WEAPON_TYPE GetCurWeapon() { return m_CurWeapon; }
    
public:
    CLONE(CMeleeAttackScript);
    CMeleeAttackScript();
    ~CMeleeAttackScript();
};

