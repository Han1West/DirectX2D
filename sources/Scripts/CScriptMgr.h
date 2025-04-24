#pragma once

#include <vector>
#include <string>

enum SCRIPT_TYPE
{
	BIKERSCRIPT,
	BLOODSCRIPT,
	BULLETSCRIPT,
	CAMERASCRIPT,
	CARSCRIPT,
	CHANGEFLOORSCRIPT,
	CURSORSCRIPT,
	DOORSCRIPT,
	EDITTILESCRIPT,
	EFFECTSCRIPT,
	ENDINGSCRIPT,
	ENEMYSCRIPT,
	FATENEMYSCRIPT,
	LEVELSCRIPT,
	MAINMENUSCRIPT,
	MASKSCRIPT,
	MELEEATTACKSCRIPT,
	MISSILESCRIPT,
	PAUSEMENUSCRIPT,
	PAUSEUISCRIPT,
	PHONESCRIPT,
	PLAYERSCRIPT,
	SELECTMASKSCRIPT,
	SHELLSCRIPT,
	SPAWNWEAPONSCRIPT,
	UISCRIPT,
	WEAPONSCRIPT,
};

using namespace std;

class CScript;

class CScriptMgr
{
public:
	static void GetScriptInfo(vector<wstring>& _vec);
	static CScript * GetScript(const wstring& _strScriptName);
	static CScript * GetScript(UINT _iScriptType);
	static const wchar_t * GetScriptName(CScript * _pScript);
};
