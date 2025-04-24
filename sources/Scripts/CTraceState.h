#pragma once
#include "CState.h"

class CTraceState :
    public CState
{
public:
    float m_Speed;
    float m_DetectRange;

    Vec3 m_DestPos;
    list<Vec2> m_lPath;
    list<Vec2>::iterator m_iter;

public:
    virtual void Enter() override;
    virtual void FinalTick() override;
    virtual void Exit() override;

public:
    CLONE(CTraceState);
    CTraceState();
    ~CTraceState();
};

