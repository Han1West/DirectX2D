#pragma once
#include "ComponentUI.h"

class TileMapUI :
    public ComponentUI
{
private:

public:
    virtual void Render_Update() override;

public:
    TileMapUI();
    ~TileMapUI();
};