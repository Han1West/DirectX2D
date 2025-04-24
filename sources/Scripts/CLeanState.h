#pragma once
#include "CState.h"

class CLeanState :
    public CState
{
public:
    float m_RecoverTime;
    float m_RecoveringTime;

public:
    virtual void Enter() override;
    virtual void FinalTick() override;
    virtual void Exit() override;
    
public:

public:
    CLONE(CLeanState);
    CLeanState();
    ~CLeanState();
};

