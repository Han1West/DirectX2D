#pragma once
#include "AssetUI.h"
class FlipbookUI :
    public AssetUI
{
private:

public:
    virtual void Render_Update() override;

public:
    FlipbookUI();
    ~FlipbookUI();
};

