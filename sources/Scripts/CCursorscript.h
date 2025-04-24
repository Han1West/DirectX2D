#pragma once
#include <Engine/CScript.h>

class CCursorscript :
    public CScript
{
private:
    bool m_bFixAim;

    CGameObject* m_Target;

public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

private:
    void BindingFixAim();

public:
    CLONE(CCursorscript);
    CCursorscript();
    ~CCursorscript();
};

