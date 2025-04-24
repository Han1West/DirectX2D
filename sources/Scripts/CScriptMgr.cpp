#include "pch.h"
#include "CScriptMgr.h"

#include "CBikerScript.h"
#include "CBloodScript.h"
#include "CBulletScript.h"
#include "CCameraScript.h"
#include "CCarScript.h"
#include "CChangeFloorScript.h"
#include "CCursorscript.h"
#include "CDoorScript.h"
#include "CEditTileScript.h"
#include "CEffectScript.h"
#include "CEndingScript.h"
#include "CEnemyScript.h"
#include "CFatEnemyScript.h"
#include "CLevelScript.h"
#include "CMainMenuScript.h"
#include "CMaskScript.h"
#include "CMeleeAttackScript.h"
#include "CMissileScript.h"
#include "CPauseMenuScript.h"
#include "CPauseUIScript.h"
#include "CPhoneScript.h"
#include "CPlayerScript.h"
#include "CSelectMaskScript.h"
#include "CShellScript.h"
#include "CSpawnWeaponScript.h"
#include "CUIScript.h"
#include "CWeaponScript.h"

void CScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CBikerScript");
	_vec.push_back(L"CBloodScript");
	_vec.push_back(L"CBulletScript");
	_vec.push_back(L"CCameraScript");
	_vec.push_back(L"CCarScript");
	_vec.push_back(L"CChangeFloorScript");
	_vec.push_back(L"CCursorscript");
	_vec.push_back(L"CDoorScript");
	_vec.push_back(L"CEditTileScript");
	_vec.push_back(L"CEffectScript");
	_vec.push_back(L"CEndingScript");
	_vec.push_back(L"CEnemyScript");
	_vec.push_back(L"CFatEnemyScript");
	_vec.push_back(L"CLevelScript");
	_vec.push_back(L"CMainMenuScript");
	_vec.push_back(L"CMaskScript");
	_vec.push_back(L"CMeleeAttackScript");
	_vec.push_back(L"CMissileScript");
	_vec.push_back(L"CPauseMenuScript");
	_vec.push_back(L"CPauseUIScript");
	_vec.push_back(L"CPhoneScript");
	_vec.push_back(L"CPlayerScript");
	_vec.push_back(L"CSelectMaskScript");
	_vec.push_back(L"CShellScript");
	_vec.push_back(L"CSpawnWeaponScript");
	_vec.push_back(L"CUIScript");
	_vec.push_back(L"CWeaponScript");
}

CScript * CScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"CBikerScript" == _strScriptName)
		return new CBikerScript;
	if (L"CBloodScript" == _strScriptName)
		return new CBloodScript;
	if (L"CBulletScript" == _strScriptName)
		return new CBulletScript;
	if (L"CCameraScript" == _strScriptName)
		return new CCameraScript;
	if (L"CCarScript" == _strScriptName)
		return new CCarScript;
	if (L"CChangeFloorScript" == _strScriptName)
		return new CChangeFloorScript;
	if (L"CCursorscript" == _strScriptName)
		return new CCursorscript;
	if (L"CDoorScript" == _strScriptName)
		return new CDoorScript;
	if (L"CEditTileScript" == _strScriptName)
		return new CEditTileScript;
	if (L"CEffectScript" == _strScriptName)
		return new CEffectScript;
	if (L"CEndingScript" == _strScriptName)
		return new CEndingScript;
	if (L"CEnemyScript" == _strScriptName)
		return new CEnemyScript;
	if (L"CFatEnemyScript" == _strScriptName)
		return new CFatEnemyScript;
	if (L"CLevelScript" == _strScriptName)
		return new CLevelScript;
	if (L"CMainMenuScript" == _strScriptName)
		return new CMainMenuScript;
	if (L"CMaskScript" == _strScriptName)
		return new CMaskScript;
	if (L"CMeleeAttackScript" == _strScriptName)
		return new CMeleeAttackScript;
	if (L"CMissileScript" == _strScriptName)
		return new CMissileScript;
	if (L"CPauseMenuScript" == _strScriptName)
		return new CPauseMenuScript;
	if (L"CPauseUIScript" == _strScriptName)
		return new CPauseUIScript;
	if (L"CPhoneScript" == _strScriptName)
		return new CPhoneScript;
	if (L"CPlayerScript" == _strScriptName)
		return new CPlayerScript;
	if (L"CSelectMaskScript" == _strScriptName)
		return new CSelectMaskScript;
	if (L"CShellScript" == _strScriptName)
		return new CShellScript;
	if (L"CSpawnWeaponScript" == _strScriptName)
		return new CSpawnWeaponScript;
	if (L"CUIScript" == _strScriptName)
		return new CUIScript;
	if (L"CWeaponScript" == _strScriptName)
		return new CWeaponScript;
	return nullptr;
}

CScript * CScriptMgr::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case (UINT)SCRIPT_TYPE::BIKERSCRIPT:
		return new CBikerScript;
		break;
	case (UINT)SCRIPT_TYPE::BLOODSCRIPT:
		return new CBloodScript;
		break;
	case (UINT)SCRIPT_TYPE::BULLETSCRIPT:
		return new CBulletScript;
		break;
	case (UINT)SCRIPT_TYPE::CAMERASCRIPT:
		return new CCameraScript;
		break;
	case (UINT)SCRIPT_TYPE::CARSCRIPT:
		return new CCarScript;
		break;
	case (UINT)SCRIPT_TYPE::CHANGEFLOORSCRIPT:
		return new CChangeFloorScript;
		break;
	case (UINT)SCRIPT_TYPE::CURSORSCRIPT:
		return new CCursorscript;
		break;
	case (UINT)SCRIPT_TYPE::DOORSCRIPT:
		return new CDoorScript;
		break;
	case (UINT)SCRIPT_TYPE::EDITTILESCRIPT:
		return new CEditTileScript;
		break;
	case (UINT)SCRIPT_TYPE::EFFECTSCRIPT:
		return new CEffectScript;
		break;
	case (UINT)SCRIPT_TYPE::ENDINGSCRIPT:
		return new CEndingScript;
		break;
	case (UINT)SCRIPT_TYPE::ENEMYSCRIPT:
		return new CEnemyScript;
		break;
	case (UINT)SCRIPT_TYPE::FATENEMYSCRIPT:
		return new CFatEnemyScript;
		break;
	case (UINT)SCRIPT_TYPE::LEVELSCRIPT:
		return new CLevelScript;
		break;
	case (UINT)SCRIPT_TYPE::MAINMENUSCRIPT:
		return new CMainMenuScript;
		break;
	case (UINT)SCRIPT_TYPE::MASKSCRIPT:
		return new CMaskScript;
		break;
	case (UINT)SCRIPT_TYPE::MELEEATTACKSCRIPT:
		return new CMeleeAttackScript;
		break;
	case (UINT)SCRIPT_TYPE::MISSILESCRIPT:
		return new CMissileScript;
		break;
	case (UINT)SCRIPT_TYPE::PAUSEMENUSCRIPT:
		return new CPauseMenuScript;
		break;
	case (UINT)SCRIPT_TYPE::PAUSEUISCRIPT:
		return new CPauseUIScript;
		break;
	case (UINT)SCRIPT_TYPE::PHONESCRIPT:
		return new CPhoneScript;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERSCRIPT:
		return new CPlayerScript;
		break;
	case (UINT)SCRIPT_TYPE::SELECTMASKSCRIPT:
		return new CSelectMaskScript;
		break;
	case (UINT)SCRIPT_TYPE::SHELLSCRIPT:
		return new CShellScript;
		break;
	case (UINT)SCRIPT_TYPE::SPAWNWEAPONSCRIPT:
		return new CSpawnWeaponScript;
		break;
	case (UINT)SCRIPT_TYPE::UISCRIPT:
		return new CUIScript;
		break;
	case (UINT)SCRIPT_TYPE::WEAPONSCRIPT:
		return new CWeaponScript;
		break;
	}
	return nullptr;
}

const wchar_t * CScriptMgr::GetScriptName(CScript * _pScript)
{
	switch ((SCRIPT_TYPE)_pScript->GetScriptType())
	{
	case SCRIPT_TYPE::BIKERSCRIPT:
		return L"CBikerScript";
		break;

	case SCRIPT_TYPE::BLOODSCRIPT:
		return L"CBloodScript";
		break;

	case SCRIPT_TYPE::BULLETSCRIPT:
		return L"CBulletScript";
		break;

	case SCRIPT_TYPE::CAMERASCRIPT:
		return L"CCameraScript";
		break;

	case SCRIPT_TYPE::CARSCRIPT:
		return L"CCarScript";
		break;

	case SCRIPT_TYPE::CHANGEFLOORSCRIPT:
		return L"CChangeFloorScript";
		break;

	case SCRIPT_TYPE::CURSORSCRIPT:
		return L"CCursorscript";
		break;

	case SCRIPT_TYPE::DOORSCRIPT:
		return L"CDoorScript";
		break;

	case SCRIPT_TYPE::EDITTILESCRIPT:
		return L"CEditTileScript";
		break;

	case SCRIPT_TYPE::EFFECTSCRIPT:
		return L"CEffectScript";
		break;

	case SCRIPT_TYPE::ENDINGSCRIPT:
		return L"CEndingScript";
		break;

	case SCRIPT_TYPE::ENEMYSCRIPT:
		return L"CEnemyScript";
		break;

	case SCRIPT_TYPE::FATENEMYSCRIPT:
		return L"CFatEnemyScript";
		break;

	case SCRIPT_TYPE::LEVELSCRIPT:
		return L"CLevelScript";
		break;

	case SCRIPT_TYPE::MAINMENUSCRIPT:
		return L"CMainMenuScript";
		break;

	case SCRIPT_TYPE::MASKSCRIPT:
		return L"CMaskScript";
		break;

	case SCRIPT_TYPE::MELEEATTACKSCRIPT:
		return L"CMeleeAttackScript";
		break;

	case SCRIPT_TYPE::MISSILESCRIPT:
		return L"CMissileScript";
		break;

	case SCRIPT_TYPE::PAUSEMENUSCRIPT:
		return L"CPauseMenuScript";
		break;

	case SCRIPT_TYPE::PAUSEUISCRIPT:
		return L"CPauseUIScript";
		break;

	case SCRIPT_TYPE::PHONESCRIPT:
		return L"CPhoneScript";
		break;

	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"CPlayerScript";
		break;

	case SCRIPT_TYPE::SELECTMASKSCRIPT:
		return L"CSelectMaskScript";
		break;

	case SCRIPT_TYPE::SHELLSCRIPT:
		return L"CShellScript";
		break;

	case SCRIPT_TYPE::SPAWNWEAPONSCRIPT:
		return L"CSpawnWeaponScript";
		break;

	case SCRIPT_TYPE::UISCRIPT:
		return L"CUIScript";
		break;

	case SCRIPT_TYPE::WEAPONSCRIPT:
		return L"CWeaponScript";
		break;

	}
	return nullptr;
}