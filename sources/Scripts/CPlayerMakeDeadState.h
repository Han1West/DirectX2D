#pragma once
#include "CState.h"

class CPlayerMakeDeadState :
    public CState
{
public:

public:
    virtual void Enter() override;
    virtual void FinalTick() override;
    virtual void Exit() override;

public:

public:
    CLONE(CPlayerMakeDeadState);
    CPlayerMakeDeadState();
    ~CPlayerMakeDeadState();
};

