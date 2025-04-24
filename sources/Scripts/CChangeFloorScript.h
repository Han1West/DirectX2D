#pragma once
#include <Engine/CScript.h>

class CChangeFloorScript :
    public CScript
{
private:
    bool m_bNext;

    Vec3 m_CollDir;

public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

public:
    void ChangeFloor();
    void ChangeFloorAtHome();

    Vec3 AdjustPlayerPos(Vec3 _Pos);
    Vec3 NormalizeDir(const Vec3& _Dir, float _SizeX, float _SizeY);

public:
    void SetbNext(bool _Next) { m_bNext = _Next; }

    bool IsNext() { return m_bNext; }

public:
    CLONE(CChangeFloorScript);
    CChangeFloorScript();
    ~CChangeFloorScript();
};

