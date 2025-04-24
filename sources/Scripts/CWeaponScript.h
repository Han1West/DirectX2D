#pragma once
#include <Engine/CScript.h>

class CWeaponScript :
    public CScript
{
private:
    bool    m_IsEquip;
    bool    m_Thrown;
    bool    m_IsDrop;
    bool    m_IsBounceOff;
    bool    m_IsGround;

    int     m_Round;
    float   m_Speed;
    float   m_AccTime;
    float   m_BounceAccTime;
    float   m_UpdateDirTime;
    
    Vec2    m_Dir;
    Vec2    m_RfDir;
    bool    m_FirstUse;
    bool    m_EnemyUse;

public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

public:
    void SetEquip(bool _Equip) { m_IsEquip = _Equip; }
    void SetThrow(bool _Throw) { m_Thrown = _Throw; }
    void SetDrop(bool _Drop) { m_IsDrop = _Drop; }
    void SetDir(Vec2 _Dir) { m_Dir = _Dir; }
    void SetRound(int _Round) { m_Round = _Round; }
    void SetFirstUse(bool _Use) { m_FirstUse = _Use; }
    void SetEnemyUse(bool _Use) { m_EnemyUse = _Use; }

    int GetRound() { return m_Round; }
    bool IsThorwing() { return m_Thrown; }
    bool IsGround() { return m_IsGround; }
    bool IsFirstUsed() { return m_FirstUse; }
    bool IsEnemyUsed() { return m_EnemyUse; }


private:
    void Throw();
    void Dropped();
    void BounceOff();
    Vec2 GetReflectionDir(Vec2 _ObjDir, float _WallTheta);
    
    void BindingGround(bool _IsGround);

public:
    CLONE(CWeaponScript);
    CWeaponScript();
    ~CWeaponScript();
};

