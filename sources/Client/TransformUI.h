#pragma once
#include "ComponentUI.h"


class TransformUI :
    public ComponentUI
{
private:

public:
    virtual void Render_Update() override;

public:
    TransformUI();
    ~TransformUI();
};

