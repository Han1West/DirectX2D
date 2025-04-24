#pragma once
#include "AssetUI.h"
class MeshUI :
    public AssetUI
{
private:

public:
    virtual void Render_Update() override;

public:
    MeshUI();
    ~MeshUI();
};

