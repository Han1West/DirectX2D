#pragma once

enum class STATE_TYPE
{
	NONE,
	IDLESTATE,
	ATTACKSTATE,
	PATROLSTATE,
	TRACESTATE,
	SEARCHSTATE,
	RETURNSTATE,
	KNOCKDOWNSTATE,
	DYINGSTATE,
	DEADSTATE,
	LEANSTATE,
	FINDSTATE,
	PLAYERMAKEDEADSTATE,
};


class CState;

class CStateMgr
{
public:
	static void GetStateInfo(vector<wstring>& _vec);
	static CState* GetState(const wstring& _strStateName);
	static CState* ChangeState(CState* _CurState ,const wstring& _strStateName);

	static CState* GetState(UINT _iScriptType);
	static const wchar_t* GetStateName(CState* _pState);
	static const wchar_t* GetStateName(STATE_TYPE _pStateType);


	static void SaveState(class CState* _State, FILE* _File);
	static void LoadState(CState*& _State, FILE* _File);
};

