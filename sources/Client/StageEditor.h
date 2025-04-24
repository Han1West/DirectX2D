#pragma once
#include "EditorUI.h"

class SE_PreloadView;
class CTexture;

class StageEditor :
    public EditorUI
{
private:
    SE_PreloadView* m_PreLoadView;

    Ptr<CTexture>  m_CurTexture;
    vector<string> m_vecList;

    bool    m_bPopup;
    bool    m_bEdit;
    bool    m_bDrag;
    bool    m_bDeco;
    bool    m_bWall;
    bool    m_bWallCorner;

    wstring m_CurAtlasName;

    Vec2    m_TileColRow;
    Vec2    m_TileSize;
    Vec2    m_TileAtlasSize;

    Vec2    m_ObjPos;
    Vec2    m_ObjScale;

    char    m_NBuffer[128];
    int     m_Idx;
    int     m_TileIdx;
    int     m_SelectedIdx;


public:
    void Init();

    virtual void Render_Update() override;

    void SetAtlas(Ptr<CTexture> _Atlas) { m_CurTexture = _Atlas; }

private:
    void ShowPopup();
    void TileUI();
    void ObjectUI();
    void ChangeMode();
    void TileEdit();
    void ObjDrag();

public:
    StageEditor();
    ~StageEditor();
};

