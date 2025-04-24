#pragma once
#include "CState.h"

class CDeadState :
    public CState
{
public:


public:


public:
    virtual void Enter() override;
    virtual void FinalTick() override;
    virtual void Exit() override;
   
public:
    CLONE(CDeadState);
    CDeadState();
    ~CDeadState();
};

