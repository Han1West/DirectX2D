#pragma once
#include "AssetUI.h"

class CTexture;

class MaterialUI :
    public AssetUI
{
private:
    char            m_Buffer[128];

    Ptr<CTexture> m_Tex;
    int           m_Idx;
    
    bool          m_TexChange;

public:
    virtual void Render_Update() override;

private:
    void SelectGraphicShader(DWORD_PTR _ListUI, DWORD_PTR _SelectString);

public:
    MaterialUI();
    ~MaterialUI();
};

