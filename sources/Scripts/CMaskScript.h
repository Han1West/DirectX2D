#pragma once
#include <Engine/CScript.h>

class CMaskScript :
    public CScript
{
private:
    Vec3      m_Scale;
    float     m_ChangingSpeed;
    float     m_AccTime;

    MASK_TYPE m_OwnType;
    int       m_OwnTypeint;

    float   m_MoveDistance;
    bool    m_Focus;


public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

public:
    void SetOwnType(int _Type) { m_OwnTypeint = _Type; }

    bool IsFocus() { return m_Focus; }
    int GetOwnTypeInt() { return m_OwnTypeint; }
    MASK_TYPE GetOwnType() { return m_OwnType; }

public:
    CLONE(CMaskScript);
    CMaskScript();
    ~CMaskScript();
};

