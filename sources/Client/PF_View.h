#pragma once
#include "PF_Sub.h"

#include <Engine/CTexture.h>

class PF_View :
    public PF_Sub
{
private:
    Ptr<CTexture>   m_AtlasTex;

    ImVec2          ImageRectMin; // Image 위젯 좌상단 실제 위치

    float           m_WidthSize;
    float           m_WheelScale;
    float           m_Ratio;        // 실제 해상도 대비 출력 이미지의 비율

public:
    virtual void Render_Update() override;


public:
    void SetAtlasTex(Ptr<CTexture> _Tex);


public:
    PF_View();
    ~PF_View();
};

