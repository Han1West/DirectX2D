#pragma once
#include "CState.h"

class CSearchState :
    public CState
{
public:
    float m_DetectRange;
    float m_AccTime;

public:
    virtual void Enter() override;
    virtual void FinalTick() override;
    virtual void Exit() override;

public:
    CLONE(CSearchState);
    CSearchState();
    ~CSearchState();
};

