#pragma once
#include <Engine/CScript.h>

class CShellScript :
    public CScript
{
private:
    float   m_Speed;
    float   m_LifeTime;
    float   m_MoveTime;
    Vec2    m_Dir;

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

public:
    CLONE(CShellScript);
    CShellScript();
    ~CShellScript();
};

