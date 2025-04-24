#pragma once
#include "SE_Sub.h"

#include <Engine/CTexture.h>
class CSprite;

class SE_Detail :
    public SE_Sub
{
private:
    Ptr<CTexture>   m_AtlasTex;

    wstring         m_SelectedFilePath;
    wstring         m_SpriteName;
    Ptr<CSprite>    m_NewSprite;
    char            m_Buffer[128];

    Vec2            m_LeftTop;
    Vec2            m_Slice;
    Vec2            m_Background;
    Vec2            m_Offset;

private:
    void Atlas();
    void AtlasInfo();
    void SpriteInfo();

public:
    void SetAtlasTex(Ptr<CTexture> _Tex);
    void SetSpriteInfo(Vec2 _LeftTop, Vec2 _RightBottom);

private:
    void SelectTexture(DWORD_PTR _ListUI, DWORD_PTR _SelectString);
    void ApplySetting(Ptr<CTexture> _Tex, Vec2 _LeftTop, Vec2 _Slice, Vec2 _Background, Vec2 _Offset);

public:
    virtual void Render_Update() override;

public:
    SE_Detail();
    ~SE_Detail();
};

