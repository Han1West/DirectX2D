#include "pch.h"
#include "CStateMgr.h"

#include "CState.h"
#include "CIdleState.h"
#include "CAttackState.h"
#include "CPatrolState.h"
#include "CTraceState.h"
#include "CSearchState.h"
#include "CReturnState.h"
#include "CKnockDownState.h"
#include "CDyingState.h"
#include "CDeadState.h"
#include "CLeanState.h"
#include "CFindState.h"

#include "CKnockDownState.h"
#include "CPlayerMakeDeadState.h"

void CStateMgr::GetStateInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"None");
	_vec.push_back(L"CIdleState");
	_vec.push_back(L"CAttackState");
	_vec.push_back(L"CPatrolState");
	_vec.push_back(L"CTraceState");
	_vec.push_back(L"CSearchState");
	_vec.push_back(L"CReturnState");
	_vec.push_back(L"CKnockDownState");
	_vec.push_back(L"CDyingState");
	_vec.push_back(L"CDeadState");
	_vec.push_back(L"CLeanState");
	_vec.push_back(L"CFindState");
	_vec.push_back(L"CPlayerMakeDeadState");
}

CState* CStateMgr::GetState(const wstring& _strScriptName)
{
	if (L"CIdleState" == _strScriptName)
		return new CIdleState;
	if (L"CAttackState" == _strScriptName)
		return new CAttackState;
	if (L"CPatrolState" == _strScriptName)
		return new CPatrolState;
	if (L"CTraceState" == _strScriptName)
		return new CTraceState;
	if (L"CSearchState" == _strScriptName)
		return new CSearchState;
	if (L"CReturnState" == _strScriptName)
		return new CReturnState;
	if (L"CKnockDownState" == _strScriptName)
		return new CKnockDownState;
	if (L"CDyingState" == _strScriptName)
		return new CDyingState;
	if (L"CDeadState" == _strScriptName)
		return new CDeadState;
	if (L"CLeanState" == _strScriptName)
		return new CLeanState;
	if (L"CFindState" == _strScriptName)
		return new CFindState;
	if (L"CPlayerMakeDeadState" == _strScriptName)
		return new CPlayerMakeDeadState;

	return nullptr;
}

CState* CStateMgr::GetState(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case (UINT)STATE_TYPE::IDLESTATE:
		return new CIdleState;
		break;
	case (UINT)STATE_TYPE::ATTACKSTATE:
		return new CAttackState;
		break;
	case (UINT)STATE_TYPE::PATROLSTATE:
		return new CPatrolState;
		break;
	case (UINT)STATE_TYPE::TRACESTATE:
		return new CTraceState;
		break;
	case (UINT)STATE_TYPE::SEARCHSTATE:
		return new CSearchState;
		break;
	case (UINT)STATE_TYPE::RETURNSTATE:
		return new CReturnState;
		break;
	case (UINT)STATE_TYPE::KNOCKDOWNSTATE:
		return new CKnockDownState;
		break;
	case (UINT)STATE_TYPE::DYINGSTATE:
		return new CDyingState;
		break;
	case (UINT)STATE_TYPE::DEADSTATE:
		return new CDeadState;
		break;
	case (UINT)STATE_TYPE::LEANSTATE:
		return new CLeanState;
		break;
	case (UINT)STATE_TYPE::FINDSTATE:
		return new CFindState;
		break;
	case (UINT)STATE_TYPE::PLAYERMAKEDEADSTATE:
		return new CPlayerMakeDeadState;
		break;
	default:
		break;
	}
	return nullptr;
}

const wchar_t* CStateMgr::GetStateName(CState* _pState)
{
	switch ((STATE_TYPE)_pState->GetStateType())
	{
	case STATE_TYPE::IDLESTATE:
		return L"CIdleState";
		break;
	case STATE_TYPE::ATTACKSTATE:
		return L"CAttackState";
		break;
	case STATE_TYPE::PATROLSTATE:
		return L"CPatrolState";
		break;
	case STATE_TYPE::TRACESTATE:
		return L"CTraceState";
		break;
	case STATE_TYPE::SEARCHSTATE:
		return L"CSearchState";
		break;
	case STATE_TYPE::RETURNSTATE:
		return L"CReturnState";
		break;
	case STATE_TYPE::KNOCKDOWNSTATE:
		return L"CKnockDownState";
		break;
	case STATE_TYPE::DYINGSTATE:
		return L"CDyingState";
		break;
	case STATE_TYPE::DEADSTATE:
		return L"CDeadState";
		break;
	case STATE_TYPE::LEANSTATE:
		return L"CLeanState";
		break;
	case STATE_TYPE::FINDSTATE:
		return L"CFindState";
		break;
	case STATE_TYPE::PLAYERMAKEDEADSTATE:
		return L"CPlayerMakeDeadState";
		break;
	default:
		break;
	}

	return nullptr;
}

const wchar_t* CStateMgr::GetStateName(STATE_TYPE _pStateType)
{
	switch (_pStateType)
	{
	case STATE_TYPE::IDLESTATE:
		return L"CIdleState";
		break;
	case STATE_TYPE::ATTACKSTATE:
		return L"CAttackState";
		break;
	case STATE_TYPE::PATROLSTATE:
		return L"CPatrolState";
		break;
	case STATE_TYPE::TRACESTATE:
		return L"CTraceState";
		break;
	case STATE_TYPE::SEARCHSTATE:
		return L"CSearchState";
		break;
	case STATE_TYPE::RETURNSTATE:
		return L"CReturnState";
		break;
	case STATE_TYPE::KNOCKDOWNSTATE:
		return L"CKnockDownState";
		break;
	case STATE_TYPE::DYINGSTATE:
		return L"CDyingState";
		break;
	case STATE_TYPE::DEADSTATE:
		return L"CDeadState";
		break;
	case STATE_TYPE::LEANSTATE:
		return L"CLeanState";
		break;
	case STATE_TYPE::FINDSTATE:
		return L"CFindState";
		break;
	case STATE_TYPE::PLAYERMAKEDEADSTATE:
		return L"CPlayerMakeDeadState";
		break;
	default:
		break;
	}

	return nullptr;
}

CState* CStateMgr::ChangeState(CState* _CurState, const wstring& _strStateName)
{
	CState* pNextState = GetState(_strStateName);

	// 기존 상태를 빠져나오고(Exit)
	if (nullptr != _CurState)
	{
		_CurState->Exit();
	}

	// 새로운 상태로 진입(Enter) 한다.
	pNextState->SetOwner(_CurState->GetOwner());
	pNextState->Enter();

	DELETE(_CurState);

	return pNextState;
}

void CStateMgr::SaveState(CState* _State, FILE* _File)
{
	if (nullptr != _State)
	{
		wstring StateName = GetStateName(_State);
		SaveWString(StateName, _File);
	}
}
		

void CStateMgr::LoadState(CState*& _State, FILE* _File)
{
	wstring StateName;

	LoadWString(StateName, _File);

	_State = GetState(StateName);
}
