#include "pch.h"
#include "ScriptUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CScript.h>
#include <Scripts/CScriptMgr.h>
#include <Scripts/CState.h>
#include <Scripts/CEnemyScript.h>
#include <Scripts/CFatEnemyScript.h>
#include <Scripts/CMaskScript.h>
#include <Scripts/CLevelScript.h>
#include <Scripts/CChangeFloorScript.h>

#include "ParamUI.h"

ScriptUI::ScriptUI()
	: ComponentUI("ScriptUI", COMPONENT_TYPE::SCRIPT)
	, m_IsEnemy(false)
	, m_IsFatEnemy(false)
	, m_IsMask(false)
	, m_IsLevel(false)
	, m_IsChangeFloor(false)
	, m_WeaponIdx(0)
	, m_StateIdx(0)
{
	m_Weapons = { "NONE", "KNIFE", "PIPE", "BAT", "M16", "DOUBLE BARREL", "PUMP SHOTGUN" };
	m_Masks = { "RICHARD", "TONY", "OBRI" , "JUAN", "GRAHAM", "DENIS", "GEORGE", "RAMI" };
}

ScriptUI::~ScriptUI()
{
}

void ScriptUI::Render_Update()
{
	wstring wScriptName = CScriptMgr::GetScriptName(m_TargetScript);
	string ScriptName = string(wScriptName.begin(), wScriptName.end());
	ComponentTitle(ScriptName.c_str());

	if (wScriptName == L"CEnemyScript")
		m_IsEnemy = true;
	else
		m_IsEnemy = false;

	if (wScriptName == L"CFatEnemyScript")
		m_IsFatEnemy = true;
	else
		m_IsFatEnemy = false;

	if (wScriptName == L"CMaskScript")
		m_IsMask = true;
	else
		m_IsMask = false;

	if (wScriptName == L"CLevelScript")
		m_IsLevel = true;
	else
		m_IsLevel = false;

	if (wScriptName == L"CChangeFloorScript")
		m_IsChangeFloor = true;
	else
		m_IsChangeFloor = false;


	const vector<tScriptParam>& vecParam = m_TargetScript->GetScriptParam();

	for (size_t i = 0; i < vecParam.size(); ++i)
	{
		switch (vecParam[i].Param)
		{
		case SCRIPT_PARAM::INT:
			ParamUI::Param_Int(vecParam[i].Desc, (int*)vecParam[i].pData, true);
			break;
		case SCRIPT_PARAM::FLOAT:
			ParamUI::Param_Float(vecParam[i].Desc, (float*)vecParam[i].pData, false);
			break;
		case SCRIPT_PARAM::VEC2:
			ParamUI::Param_Vec2(vecParam[i].Desc, (Vec2*)vecParam[i].pData, false);
			break;
		case SCRIPT_PARAM::VEC4:
			ParamUI::Param_Vec4(vecParam[i].Desc, (Vec4*)vecParam[i].pData, false);
			break;
		case SCRIPT_PARAM::TEXTURE:
			ParamUI::Param_Tex(vecParam[i].Desc, *((Ptr<CTexture>*)vecParam[i].pData));
			break;
		case SCRIPT_PARAM::PREFAB:
			ParamUI::Param_Prefab(vecParam[i].Desc, *((Ptr<CPrefab>*)vecParam[i].pData));
			break;
		}
	}

	if (m_IsEnemy)
	{
		FindEnemyScript();
		SetWeaponUI();
		SetStateUI();
	}

	if (m_IsFatEnemy)
	{
		FindEnemyScript();
		SetStateUI();
	}

	if (m_IsMask)
	{
		FindMaskScript();
		SetMaskUI();
	}

	if (m_IsLevel)
	{
		FindLevelScript();
		SetLevelUI();
		SetNextLevelUI();
	}

	if (m_IsChangeFloor)
	{
		FindChangeFloorScript();
		SetbNextUI();
	}
}

void ScriptUI::SetScript(CScript* _Script)
{
	m_TargetScript = _Script;

	if (nullptr == m_TargetScript)
	{
		SetTargetObject(nullptr);
		SetActive(false);
	}
	else
	{
		SetTargetObject(m_TargetScript->GetOwner());
		SetActive(true);
	}
}

void ScriptUI::SetWeaponUI()
{

	ImGui::Text("Cur Weapon");
	ImGui::SameLine(120);
	ImGui::SetNextItemWidth(150);

	int FirstIdx = 0;
	
	switch ((int)m_EnemyScript->GetWeaponType())
	{
	case 0:
		FirstIdx = 0;
		break;
	case 1:
		FirstIdx = 1;
		break;
	case 2:
		FirstIdx = 2;
		break;
	case 3:
		FirstIdx = 3;
		break;
	case 6:
		FirstIdx = 4;
		break;
	case 9:
		FirstIdx = 5;
		break;
	case 10:
		FirstIdx = 6;
		break;
	}


	//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	//ImGui::Text(m_Buffer);
	//ImGui::PopStyleColor();

	// 콤보박스 생성
 	if (ImGui::BeginCombo("##Select Weapon", m_Weapons[FirstIdx].c_str()))
	{
		for (int i = 0; i < m_Weapons.size(); ++i)
		{
			bool isSelected = (m_WeaponIdx == i);

			if (ImGui::Selectable(m_Weapons[i].c_str(), isSelected))
			{
				m_WeaponIdx = i;
			}
			// 선택된 항목 강조
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// 고른 Weapon 을 스크립트에 적용
	ImGui::SameLine(290);
	if (ImGui::Button("Apply##Weapon"))
	{
		m_EnemyScript->SetWeapon(m_WeaponIdx);
	}
}

void ScriptUI::SetStateUI()
{
	ImGui::Text("Cur State");
	ImGui::SameLine(120);
	ImGui::SetNextItemWidth(150);
	
	// 현재 State Idx 를 받아온다
	int FirstIdx = 0;
	if(m_IsEnemy)
		FirstIdx = (int)m_EnemyScript->GetStateType();
	if (m_IsFatEnemy)
		FirstIdx = (int)m_FatEnemyScript->GetStateType();

	// State목록을 vector에 저장
	vector<wstring> vecStateNames;
	CStateMgr::GetStateInfo(vecStateNames);

	// 콤보 박스 생성
	if (ImGui::BeginCombo("##Select State", string(vecStateNames[FirstIdx].begin(), vecStateNames[FirstIdx].end()).c_str()))
	{
		for (int i = 0; i < vecStateNames.size(); ++i)
		{
			bool isSelected = (m_StateIdx == i);

			if (ImGui::Selectable(string(vecStateNames[i].begin(), vecStateNames[i].end()).c_str(), isSelected))
			{
				m_StateIdx = i;
			}
			// 선택된 항목 강조
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// 선택한 State를 Script에 적용
	ImGui::SameLine(290);
	if (ImGui::Button("Apply##State"))
	{
		if(m_IsEnemy)
			m_EnemyScript->SetState(CStateMgr::GetState(vecStateNames[m_StateIdx]));
		if(m_IsFatEnemy)
			m_FatEnemyScript->SetState(CStateMgr::GetState(vecStateNames[m_StateIdx]));
	}
}

void ScriptUI::SetMaskUI()
{
	ImGui::Text("Cur Mask");
	ImGui::SameLine(120);
	ImGui::SetNextItemWidth(150);

	// 현재 Weapon의 Idx를 받아온다.
	int FirstIdx = (int)m_MaskScript->GetOwnTypeInt();

	// 콤보박스 생성
	if (ImGui::BeginCombo("##Select Weapon", m_Masks[FirstIdx].c_str()))
	{
		for (int i = 0; i < m_Masks.size(); ++i)
		{
			bool isSelected = (m_MaskIdx == i);

			if (ImGui::Selectable(m_Masks[i].c_str(), isSelected))
			{
				m_MaskIdx = i;
			}
			// 선택된 항목 강조
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// 고른 Weapon 을 스크립트에 적용
	ImGui::SameLine(290);
	if (ImGui::Button("Apply##Mask"))
	{
		m_MaskScript->SetOwnType(m_MaskIdx);
	}
}


void ScriptUI::SetLevelUI()
{
	ImGui::Text("Cur Floor");
	ImGui::SameLine(120);
	ImGui::SetNextItemWidth(150);

	int Floor = m_LevelScript->GetFloor();

	if (ImGui::InputInt("##SetFloor", &Floor))
	{
		m_LevelScript->SetFloor(Floor);
	}

	ImGui::Text("Set Start");
	ImGui::SameLine(120);
	ImGui::SetNextItemWidth(150);

	bool Start = m_LevelScript->IsStart();

	if (ImGui::Checkbox("##SetStart", &Start))
	{
		m_LevelScript->SetStart(Start);
	}

	ImGui::Text("Set Final");
	ImGui::SameLine(120);
	ImGui::SetNextItemWidth(150);

	bool Final = m_LevelScript->IsFinal();

	if (ImGui::Checkbox("##SetFinal", &Final))
	{
		m_LevelScript->SetFinal(Final);
	}

	ImGui::Text("Set Home");
	ImGui::SameLine(120);
	ImGui::SetNextItemWidth(150);

	bool Home = m_LevelScript->IsHome();

	if (ImGui::Checkbox("##SetHome", &Home))
	{
		m_LevelScript->SetHome(Home);
	}
}

void ScriptUI::SetNextLevelUI()
{
	ImGui::Text("Next Level");
	ImGui::SameLine(120);
	ImGui::SetNextItemWidth(150);

	wstring CurNext = m_LevelScript->GetNextLevelPath();
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	ImGui::Text(string(CurNext.begin(), CurNext.end()).c_str());
	ImGui::PopStyleColor();

	string NextTemp = "";
	ImGui::InputText("##NextLevelPath", m_BufferNext, IM_ARRAYSIZE(m_BufferNext));
	if (ImGui::Button("Apply##Next"))
	{
		NextTemp = m_BufferNext;
		m_LevelScript->SetNextLevelPath(wstring(NextTemp.begin(), NextTemp.end()));
	}

	ImGui::Text("Prev Level");
	ImGui::SameLine(120);
	ImGui::SetNextItemWidth(150);

	wstring CurPrev = m_LevelScript->GetPrevLevelPath();
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	ImGui::Text(string(CurPrev.begin(), CurPrev.end()).c_str());
	ImGui::PopStyleColor();

	string PrevTemp = "";
	ImGui::InputText("##PrevLevelPath", m_BufferPrev, IM_ARRAYSIZE(m_BufferPrev));
	if (ImGui::Button("Apply##Prev"))
	{
		PrevTemp = m_BufferPrev;
		m_LevelScript->SetPrevLevelPath(wstring(PrevTemp.begin(), PrevTemp.end()));
	}
	
}

void ScriptUI::SetbNextUI()
{
	ImGui::Text("Set Bool Next");
	ImGui::SameLine(120);
	ImGui::SetNextItemWidth(150);

	bool Next = m_ChangeFloorScript->IsNext();

	if (ImGui::Checkbox("##SetbNext", &Next))
	{
		m_ChangeFloorScript->SetbNext(Next);
	}
}

void ScriptUI::FindEnemyScript()
{
	vector<CScript*> pScripts = GetTargetObject()->GetScripts();
	for (int i = 0; i < pScripts.size(); ++i)
	{
		if (pScripts[i]->GetScriptType() == SCRIPT_TYPE::ENEMYSCRIPT)
			m_EnemyScript = dynamic_cast<CEnemyScript*>(pScripts[i]);
		if (pScripts[i]->GetScriptType() == SCRIPT_TYPE::FATENEMYSCRIPT)
			m_FatEnemyScript = dynamic_cast<CFatEnemyScript*>(pScripts[i]);
	}
}

void ScriptUI::FindMaskScript()
{
	vector<CScript*> pScripts = GetTargetObject()->GetScripts();
	for (int i = 0; i < pScripts.size(); ++i)
	{
		if (pScripts[i]->GetScriptType() == SCRIPT_TYPE::MASKSCRIPT)
			m_MaskScript = dynamic_cast<CMaskScript*>(pScripts[i]);
	}
}

void ScriptUI::FindLevelScript()
{
	vector<CScript*> pScripts = GetTargetObject()->GetScripts();
	for (int i = 0; i < pScripts.size(); ++i)
	{
		if (pScripts[i]->GetScriptType() == SCRIPT_TYPE::LEVELSCRIPT)
			m_LevelScript = dynamic_cast<CLevelScript*>(pScripts[i]);
	}
}


void ScriptUI::FindChangeFloorScript()
{
	vector<CScript*> pScripts = GetTargetObject()->GetScripts();
	for (int i = 0; i < pScripts.size(); ++i)
	{
		if (pScripts[i]->GetScriptType() == SCRIPT_TYPE::CHANGEFLOORSCRIPT)
			m_ChangeFloorScript = dynamic_cast<CChangeFloorScript*>(pScripts[i]);
	}
}
