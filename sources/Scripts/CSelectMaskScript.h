#pragma once
#include <Engine/CScript.h>

class CSelectMaskScript :
    public CScript
{
private:
    bool      m_Active;
    bool      m_Changing;
    bool      m_IsUp;

    float     m_ChangingTime;
    float     m_AccTime;

    int       m_Idx;
    MASK_TYPE m_SelectedMask;

public:
    virtual void Tick() override;
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

public:
    bool IsActive() { return m_Active; }
    bool IsChanging() { return m_Changing; }
    bool IsUp() { return m_IsUp; }

    int GetCurIndex() { return m_Idx; }


private:

public:
    CLONE(CSelectMaskScript);
    CSelectMaskScript();
    ~CSelectMaskScript();
};

