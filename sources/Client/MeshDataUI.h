#pragma once
#include "AssetUI.h"
class MeshDataUI :
    public AssetUI
{
private:

public:
    virtual void Render_Update() override;

public:
    MeshDataUI();
    ~MeshDataUI();
};

