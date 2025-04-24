#include "pch.h"
#include "CEditTileScript.h"

#include <Engine/CKeyMgr.h>
#include <Engine/CTileMap.h>
#include <Engine/CRenderMgr.h>

#include <Engine/CDevice.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CConstBuffer.h>

CEditTileScript::CEditTileScript()
	: CScript(SCRIPT_TYPE::EDITTILESCRIPT)
	, m_Active(false)
	, m_IsChanged(true)
{
}

CEditTileScript::~CEditTileScript()
{
}


void CEditTileScript::Begin()
{
}

void CEditTileScript::Tick()
{

}

void CEditTileScript::FinalTick()
{
	if (m_Active)
	{
		Vec2 ColRow = TileMap()->GetTileColRow();

		int COL = ColRow.x;
		int ROW = ColRow.y;

		Vec2 TileSize = TileMap()->GetTileSize();
		Vec3 TilePos = Transform()->GetRelativePos();

		float TotalWidth = TileSize.x * COL;
		float TotalHeight = TileSize.y * ROW;

		Vec3 TopLeft = Vec3(TilePos.x - TotalWidth * 0.5f
			, TilePos.y + TotalHeight * 0.5f
			, TilePos.z);

		// 타일 위치에 Rect를 그린다
		if (m_IsChanged)
		{
			tTileDebug Debug = {};
			Debug.bTileDebug = true;

			CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::TILEDEBUG);

			pCB->SetData(&Debug);
			pCB->Binding();

			m_IsChanged = false;
		}

		Vec3 MousePos = CKeyMgr::GetInst()->GetRelativeMousePos();

		// 해당하는 타일에 마우스 클릭 체크

		for (UINT i = 0; i < COL; ++i)
		{
			for (UINT j = 0; j < ROW; ++j)
			{
				Vec3 RectPos = Vec3(TopLeft.x + TileSize.x * i + TileSize.x / 2.f
					, TopLeft.y - TileSize.y * (j + 1) + TileSize.y / 2.f
					, TilePos.z);

				if (KEY_PRESSED(KEY::LBTN))
				{
					if (RectPos.x - TileSize.x / 2.f < MousePos.x && MousePos.x < RectPos.x + TileSize.x / 2.f
						&& RectPos.y - TileSize.y / 2.f < MousePos.y && MousePos.y < RectPos.y + TileSize.y / 2.f) 
					{
						int Idx = (j * COL) + i;

						TileMap()->SetTileInfo(m_LeftTop, m_Slice, Idx);
					}
				}
			}
		}
	}

	else
	{
		if (m_IsChanged)
		{
			tTileDebug Debug = {};
			Debug.bTileDebug = false;

			CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::TILEDEBUG);

			pCB->SetData(&Debug);
			pCB->Binding();

			m_IsChanged = false;
		}
		//CRenderMgr::GetInst()->DeleteDbgObj(L"Tile");
	}
}

void CEditTileScript::SaveComponent(FILE* _File)
{
}

void CEditTileScript::LoadComponent(FILE* _File)
{
}
