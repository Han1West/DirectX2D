#pragma once
#include "AssetUI.h"
class TextureUI :
    public AssetUI
{
private:

public:
    virtual void Render_Update() override;

public:
    TextureUI();
    ~TextureUI();
};

