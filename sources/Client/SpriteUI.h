#pragma once
#include "AssetUI.h"
class SpriteUI :
    public AssetUI
{
private:

public:
    virtual void Render_Update() override;

public:
    SpriteUI();
    ~SpriteUI();
};

