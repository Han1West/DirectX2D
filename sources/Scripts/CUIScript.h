#pragma once
#include <Engine/CScript.h>

class CUIScript :
    public CScript
{
private:
    float m_AccTime;
    float m_ClearAccTime;

    Vec3 m_UIScale;

public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void Overlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObject, CCollider2D* _OtherCollider);
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

public:
    void SetCurNumFlip();
    void SetActive();
    void SetDeadActive();
    void SetDeadBackgroundActive();
    void SetCurMaskFlip();
    void SetClearUI();
    void SetActiveArrowUI();

    void MainMenuUI();
    void BindingSelect(bool _StartSelect, bool _ExitSelect);

public:
    CLONE(CUIScript);
    CUIScript();
    ~CUIScript();
};

