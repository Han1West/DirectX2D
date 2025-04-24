#pragma once
#include "AssetUI.h"
class PrefabUI :
    public AssetUI
{
private:

public:
    virtual void Render_Update() override;

public:
    PrefabUI();
    ~PrefabUI();
};

