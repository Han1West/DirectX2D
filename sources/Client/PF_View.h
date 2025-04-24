#pragma once
#include "PF_Sub.h"

#include <Engine/CTexture.h>

class PF_View :
    public PF_Sub
{
private:
    Ptr<CTexture>   m_AtlasTex;

    ImVec2          ImageRectMin; // Image ���� �»�� ���� ��ġ

    float           m_WidthSize;
    float           m_WheelScale;
    float           m_Ratio;        // ���� �ػ� ��� ��� �̹����� ����

public:
    virtual void Render_Update() override;


public:
    void SetAtlasTex(Ptr<CTexture> _Tex);


public:
    PF_View();
    ~PF_View();
};

