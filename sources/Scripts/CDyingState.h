#pragma once
#include "CState.h"

class CDyingState :
    public CState
{
private:

public:
    virtual void Enter() override;
    virtual void FinalTick() override;
    virtual void Exit() override;

public:
    CLONE(CDyingState);
    CDyingState();
    ~CDyingState();
};

