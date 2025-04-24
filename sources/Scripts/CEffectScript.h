#pragma once
#include <Engine/CScript.h>

class CEffectScript :
    public CScript
{
private:
    float m_AccTime;


public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

public:


public:
    CLONE(CEffectScript);
    CEffectScript();
    ~CEffectScript();
};

