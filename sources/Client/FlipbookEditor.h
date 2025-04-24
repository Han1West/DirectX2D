#pragma once
#include "EditorUI.h"

class CSprite;
class CTexture;
class CFlipbook;

class FlipbookEditor :
    public EditorUI
{
private:
    Ptr<CSprite>    m_LoadedSprite;
    Ptr<CTexture>   m_CurrentTexture;
    Ptr<CFlipbook>  m_NewFlipbook;
    char            m_SPBuffer[128];
    char            m_FLBuffer[128];

    Vec2            m_OffsetUV;
    int             m_Idx;
    bool            m_Play;
    float           m_Time;

public:
    virtual void Render_Update() override;

public:
    FlipbookEditor();
    ~FlipbookEditor();
};

