#pragma once
#include "ComponentUI.h"

class CFlipbook;

class FlipbookPlayerUI :
    public ComponentUI
{
private:
    int m_FlipbookID;
    int m_Idx;
    
    bool    m_FlipbookChanged;
    vector<string> m_vecFlipNames;

public:
    virtual void Render_Update() override;

    void SetFlipbookChange(bool _Change) { m_FlipbookChanged = _Change; }

private:
    void SelectFlipbook(DWORD_PTR _ListUI, DWORD_PTR _SelectString);

    void UpdateFlipbookName(vector<Ptr<CFlipbook>> _CurFlipbook);

public:
    FlipbookPlayerUI();
    ~FlipbookPlayerUI();
};

