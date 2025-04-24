#pragma once
#include <Engine/CScript.h>

class CBloodScript :
    public CScript
{
private:
    float   m_Speed;
    float   m_AccTime;
    float   m_SpawnTime;
    Vec2    m_Dir;

    bool    m_IsSpread;
    bool    m_IsBase;

public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

public:
    void SetDir(Vec2 _Dir) { m_Dir = _Dir; }
    void SetSpread(bool _Spread) { m_IsSpread = _Spread; }
    void SetBase(bool _Base) { m_IsBase = _Base; }

public:
    Vec2 RotateVector(const Vec2& _vec, float Angle);

    void Spread();
    void MakeSplat();

public:
    CLONE(CBloodScript);
    CBloodScript();
    ~CBloodScript();
};

