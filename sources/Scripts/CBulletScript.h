#pragma once
#include <Engine/CScript.h>

class CBulletScript :
    public CScript
{
private:
    float   m_Speed;
    Vec2    m_Dir;

    bool    m_IsShotgun;
    bool    m_IsEnemy;

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
    void SetShotgun(bool _IsShotgun) { m_IsShotgun = _IsShotgun; }
    void SetEnemy(bool _Enemy) { m_IsEnemy = _Enemy; }

    Vec2 RotateVector(const Vec2& _vec, float Angle);
    bool IsShotgun() { return m_IsShotgun; }
public:
    CLONE(CBulletScript);
    CBulletScript();
    ~CBulletScript();
};

