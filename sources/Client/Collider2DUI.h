#pragma once
#include "ComponentUI.h"

class Collider2DUI :
    public ComponentUI
{
private:

public:
    virtual void Render_Update() override;

    char    m_Buffer[128];
   
public:
    Collider2DUI();
    ~Collider2DUI();
};

