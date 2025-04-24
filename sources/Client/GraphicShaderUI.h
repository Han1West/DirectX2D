#pragma once
#include "AssetUI.h"
class GraphicShaderUI :
    public AssetUI
{
private:

public:
    virtual void Render_Update() override;

public:
    GraphicShaderUI();
    ~GraphicShaderUI();
};

