#pragma once
#include "ComponentUI.h"


class CameraUI :
    public ComponentUI
{
private:


public:
    virtual void Render_Update() override;

public:
    CameraUI();
    ~CameraUI();
};

