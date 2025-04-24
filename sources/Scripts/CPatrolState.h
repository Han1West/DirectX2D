#pragma once
#include "CState.h"

class CPatrolState :
    public CState
{
public:
    float m_Speed;

    float m_DetectRange;
    float m_WallDetectRange;

    float m_FrontWallDis;

public:
    virtual void Enter() override;
    virtual void FinalTick() override;
    virtual void Exit() override;

public:
    CLONE(CPatrolState);
    CPatrolState();
    ~CPatrolState();
};

