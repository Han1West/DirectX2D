#pragma once

#include "AssetUI.h"

class ComputeShaderUI :
    public AssetUI
{
private:

public:
    virtual void Render_Update() override;

public:
    ComputeShaderUI();
    ~ComputeShaderUI();
};

