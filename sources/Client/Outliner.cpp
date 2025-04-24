#include "pch.h"
#include "Outliner.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CLayer.h>
#include <Engine/CGameObject.h>
#include <Engine/CTaskMgr.h>

#include "CImGuiMgr.h"
#include "Inspector.h"
#include "TreeUI.h"


Outliner::Outliner()
	: EditorUI("Outliner")
{
	m_Tree = (TreeUI*)AddChildUI(new TreeUI);
	m_Tree->SetName("Outliner");
	m_Tree->ShowRoot(false);
	m_Tree->SelfDragDrop(true);

	m_Tree->AddDynamicSelect(this, (EUI_DELEGATE_1) &Outliner::SelectGameObject);
	m_Tree->AddDynamicDelete(this, (EUI_DELEGATE_1)&Outliner::DeleteNodeObject);
	m_Tree->AddDynamicSelfDragDrop(this, (EUI_DELEGATE_2)&Outliner::DragDrop);
	
}

Outliner::~Outliner()
{
}

void Outliner::Render_Update()
{
	if (CTaskMgr::GetInst()->IsLevelChanged())
	{
		RenewGameObject();
	}
}

void Outliner::RenewGameObject()
{
	m_Tree->Clear();
	TreeNode* pRootNode = m_Tree->AddItem(nullptr, "RootNode", 0);

	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pCurLevel->GetLayer(i);

		const vector<CGameObject*>& vecParents = pLayer->GetParentObjects();
		for (size_t j = 0; j < vecParents.size(); ++j)
		{
			AddGameObject(pRootNode, vecParents[j]);
		}
	}	
}

void Outliner::AddGameObject(TreeNode* _ParentNode, CGameObject* _Object)
{
	string Name = string(_Object->GetName().begin(), _Object->GetName().end());

	TreeNode* pCurNode = m_Tree->AddItem(_ParentNode, Name, (DWORD_PTR)_Object);

	const vector<CGameObject*>& vecChild = _Object->GetChild();

	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		AddGameObject(pCurNode, vecChild[i]);
	}
}

void Outliner::SelectGameObject(DWORD_PTR _TreeNode)
{
	TreeNode* pNode = (TreeNode*)_TreeNode;
	CGameObject* pTarget = (CGameObject*)pNode->GetData();

	Inspector* pInspector = (Inspector*)CImGuiMgr::GetInst()->FindUI("Inspector");
	pInspector->SetTargetObject(pTarget);
}

void Outliner::DeleteNodeObject(DWORD_PTR _TreeNode)
{
	TreeNode* pNode = (TreeNode*)_TreeNode;
	CGameObject* pTarget = (CGameObject*)pNode->GetData();

	DestroyObject(pTarget);
}

void Outliner::DragDrop(DWORD_PTR _DragNode, DWORD_PTR _DropNode)
{
	TreeNode* pDragged = (TreeNode*)_DragNode;
	TreeNode* pDropped = (TreeNode*)_DropNode;

	CGameObject* pDragObj = (CGameObject*)pDragged->GetData();
	CGameObject* pDropObj = nullptr;
	
	if (pDropped)
	{
		pDropObj = (CGameObject*)pDropped->GetData();

		// 드래그 된 오브젝트가 목적지 오브젝트의 조상 중 하나라면, 그 상황을 방지한다.
		if (pDropObj->IsAncestor(pDragObj))
			return;
	}	

	AddChild(pDropObj, pDragObj);
}
