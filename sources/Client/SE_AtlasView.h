#pragma once
#include "SE_Sub.h"

#include <Engine/CTexture.h>

class SE_AtlasView :
    public SE_Sub
{
private:
    Ptr<CTexture>   m_AtlasTex;

    ImVec2          ImageRectMin; // Image 위젯 좌상단 실제 위치

    ImVec2          m_MousePos; // 실제 마우스 위치

    ImVec2          m_MouseLT;  // 마우스 위치에 해당하는 Texture 의 픽셀 좌표
    ImVec2          m_MouseRB; // 마우스 위치에 해당하는 Texture 의 픽셀 좌표

    set<Vec2>       m_PixelID;  // 등록된적이 있는 픽셀인지 확인하는 용도

    float           m_WidthSize;
    float           m_WheelScale;
    float           m_Ratio;        // 실제 해상도 대비 출력 이미지의 비율

public:
    virtual void Render_Update() override;


public:
    void SetAtlasTex(Ptr<CTexture> _Tex);
    void SetRectPos(Vec2 _LeftTop, Vec2 _Slice);

private:
    void WheelCheck();
    void SelectCheck();
    void DrawSelectRect();
    void CalcSpriteSize(Vec2 _PixelPos);
    bool IsPixelOk(Vec2 _PixelPos);

public:
    SE_AtlasView();
    ~SE_AtlasView();
};

