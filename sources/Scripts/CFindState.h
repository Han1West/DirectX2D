#pragma once
#include "CState.h"


class CFindState :
    public CState
{
    float m_Speed;
    float m_DetectRange;


public:
    virtual void Enter() override;
    virtual void FinalTick() override;
    virtual void Exit() override;

public:
    CLONE(CFindState);
    CFindState();
    ~CFindState();
};

