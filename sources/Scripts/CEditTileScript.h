#pragma once
#include <Engine/CScript.h>

class CEditTileScript :
    public CScript
{
private:
    bool    m_Active;
    bool    m_IsChanged;

    Vec2    m_LeftTop;
    Vec2    m_Slice;

public:
    virtual void Begin() override;
    virtual void Tick() override;
    virtual void FinalTick() override;
    virtual void SaveComponent(FILE* _File) override;
    virtual void LoadComponent(FILE* _File) override;

public:
    void SetActive(bool _Active) { m_Active = _Active; }
    
    bool IsActive() { return m_Active; }

    void SetChange(bool _Change) { m_IsChanged = _Change; }

    
    void SetTileInfo(Vec2 _LeftTop, Vec2 _Slice) { m_LeftTop = _LeftTop; m_Slice = _Slice; }

public:
    CLONE(CEditTileScript);
    CEditTileScript();
    ~CEditTileScript();
};

