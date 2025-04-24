#include "pch.h"
#include "func.h"

#include "CRenderMgr.h"
#include "CTaskMgr.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CGameObject.h"
#include "components.h"

void CreateObject(CGameObject* _NewObj, int LayerIdx, bool _bMoveChild)
{
	tTask task = {};
	task.Type = TASK_TYPE::CREATE_OBJECT;
	task.Param0 = (DWORD_PTR)_NewObj;
	task.Param1 = LayerIdx;
	task.Param2 = _bMoveChild;

	CTaskMgr::GetInst()->AddTask(task);
}

void DestroyObject(CGameObject* _TargetObj)
{
	tTask task = {};
	task.Type = TASK_TYPE::DELETE_OBJECT;
	task.Param0 = (DWORD_PTR)_TargetObj;

	CTaskMgr::GetInst()->AddTask(task);
}

void ChangeLevelState(LEVEL_STATE _NextState)
{
	tTask task = {};
	task.Type = TASK_TYPE::CHANGE_LEVEL_STATE;
	task.Param0 = (DWORD_PTR)_NextState;	

	CTaskMgr::GetInst()->AddTask(task);
}

void ChangeLevel(CLevel* _Level, LEVEL_STATE _NextState)
{
	tTask task = {};

	task.Type = TASK_TYPE::CHANGE_LEVEL;
	task.Param0 = (DWORD_PTR)_Level;
	task.Param1 = (DWORD_PTR)_NextState;

	CTaskMgr::GetInst()->AddTask(task);
}

void AddChild(CGameObject* _Parent, CGameObject* _Child)
{
	tTask task = {};

	task.Type = TASK_TYPE::ADD_CHILD;
	task.Param0 = (DWORD_PTR)_Parent;
	task.Param1 = (DWORD_PTR)_Child;

	CTaskMgr::GetInst()->AddTask(task);
}

void DrawDebugRect(Vec4 _Color, Vec3 _Pos, Vec2 _Scale, Vec3 _Rotation, bool _DepthTest, float _Duration, wstring _Name)
{
	tDebugShapeInfo info = {};
	info.Name = _Name;

	info.Shape = DEBUG_SHAPE::RECT;
	info.Color = _Color;

	info.WorldPos = _Pos;
	info.Scale = Vec3(_Scale.x, _Scale.y, 1.f);	
	info.Rotation = _Rotation;
	info.matWorld = XMMatrixIdentity();

	info.DepthTest = _DepthTest;
	info.Duration = _Duration;
	CRenderMgr::GetInst()->AddDebugShape(info);
}

void DrawDebugRect(Vec4 _Color, const Matrix& _matWorld, bool _DepthTest, float _Duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::RECT;
	info.Color = _Color;
	
	info.matWorld = _matWorld;

	info.DepthTest = _DepthTest;
	info.Duration = _Duration;
	CRenderMgr::GetInst()->AddDebugShape(info);
}

void DrawDebugCircle(Vec4 _Color, Vec3 _Pos, float _Radius, bool _DepthTest, float _Duration)
{
	tDebugShapeInfo info = {};
	info.Shape = DEBUG_SHAPE::CIRCLE;
	info.Color = _Color;
	info.WorldPos = _Pos;
	info.Scale = Vec3(_Radius, _Radius, 1.f);
	info.DepthTest = _DepthTest;
	info.Duration = _Duration;
	CRenderMgr::GetInst()->AddDebugShape(info);
}

void SaveWString(const wstring& _str, FILE* _File)
{
	size_t Len = _str.length();
	fwrite(&Len, sizeof(size_t), 1, _File);
	fwrite(_str.c_str(), sizeof(wchar_t), Len, _File);
}

void LoadWString(wstring& _str, FILE* _File)
{
	wchar_t szBuffer[255] = {};

	size_t Len = 0;
	fread(&Len, sizeof(size_t), 1, _File);
	fread(szBuffer, sizeof(wchar_t), Len, _File);
	_str = szBuffer;
}

void GetComponentsNames(vector<wstring>& _vecComponentsNames)
{
	_vecComponentsNames.push_back(L"Transform");
	_vecComponentsNames.push_back(L"Collider2D");
	_vecComponentsNames.push_back(L"FlipbookPlayer");
	_vecComponentsNames.push_back(L"Camera");
	_vecComponentsNames.push_back(L"Light2D");
	_vecComponentsNames.push_back(L"StateMachine");
	_vecComponentsNames.push_back(L"MeshRender");
	_vecComponentsNames.push_back(L"Tilemap");
	_vecComponentsNames.push_back(L"ParticleSystem");
	//_vecComponentsNames.push_back(L"Script");	
}

vector<CGameObject*> FindWallBetween(Vec2 _Pos1, Vec2 _Pos2)
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	vector<CGameObject*> vecList;
	vector<CGameObject*> vecWallList;

	vector<CGameObject*> vecObject = pCurLevel->GetLayer(1)->GetParentObjects();

	for (UINT i = 0; i < vecObject.size(); ++i)
	{
		if (vecObject[i]->Collider2D())
		{
			if (vecObject[i]->Collider2D()->GetName() == L"Wall" || vecObject[i]->GetName() == L"Door")
				vecList.push_back(vecObject[i]);
		}
	}

	for (UINT i = 0; i < vecList.size(); ++i)
	{
		Vec3 vPos = vecList[i]->Transform()->GetRelativePos();
		Vec3 vRot = vecList[i]->Transform()->GetRelativeRotation();

		if (vecList[i]->GetName() == L"Door")
		{
			// 수평
			if (vRot.z == 0.f)
			{
				if ((vPos.y < _Pos1.y && _Pos2.y < vPos.y)
					|| (vPos.y < _Pos2.y && _Pos1.y < vPos.y))
				{
					vecWallList.push_back(vecList[i]);
				}
			}

			// 수직
			else
			{
				if ((vPos.x < _Pos1.x && _Pos2.x < vPos.x)
					|| (vPos.x < _Pos2.x && _Pos1.x < vPos.x))
				{
					vecWallList.push_back(vecList[i]);
				}
			}
		}

		else
		{
			// 수직
			if (vRot.z == 0.f)
			{
				if ((vPos.x < _Pos1.x && _Pos2.x < vPos.x)
					|| (vPos.x < _Pos2.x && _Pos1.x < vPos.x))
				{
					vecWallList.push_back(vecList[i]);
				}
			}

			// 수평
			else
			{
				if ((vPos.y < _Pos1.y && _Pos2.y < vPos.y)
					|| (vPos.y < _Pos2.y && _Pos1.y < vPos.y))
				{
					vecWallList.push_back(vecList[i]);
				}
			}
		}
	}

	return vecWallList;
}

vector<CGameObject*> FindWallAround(Vec2 _Pos1, float _Range)
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	vector<CGameObject*> vecList;
	vector<CGameObject*> vecWallList;

	vector<CGameObject*> vecObject = pCurLevel->GetLayer(1)->GetParentObjects();
	vecObject.insert(vecObject.end(), pCurLevel->GetLayer(8)->GetParentObjects().begin(), pCurLevel->GetLayer(8)->GetParentObjects().end());

	for (UINT i = 0; i < vecObject.size(); ++i)
	{
		if (vecObject[i]->Collider2D())
		{
			if (vecObject[i]->Collider2D()->GetName() == L"Wall" || vecObject[i]->Collider2D()->GetName() == L"StgObj")
				vecList.push_back(vecObject[i]);
		}		
	}

	for (UINT i = 0; i < vecList.size(); ++i)
	{
		Vec3 vPos = vecList[i]->Transform()->GetRelativePos();
		Vec3 vScale = vecList[i]->Transform()->GetRelativeScale();
		Vec3 vRot = vecList[i]->Transform()->GetRelativeRotation();

		// 수직
		if (vRot.z == 0.f)
		{
			if ((vPos.x < _Pos1.x+ _Range && _Pos1.x - _Range < vPos.x)
				&& ((_Pos1.y + _Range < vPos.y + vScale.y / 2.f && vPos.y - vScale.y / 2.f < _Pos1.y - _Range)
				|| (vPos.y + vScale.y / 2.f < _Pos1.y + _Range && _Pos1.y - _Range < vPos.y + vScale.y / 2.f)
				|| (vPos.y - vScale.y / 2.f < _Pos1.y + _Range && _Pos1.y - _Range < vPos.y - vScale.y / 2.f)))
			{
				vecWallList.push_back(vecList[i]);
			}			
		}

		// 수평
		else
		{
			//bool a = (vPos.y < _Pos1.y + _Range && _Pos1.y - _Range < vPos.y);
			//bool b = (_Pos1.x + _Range < vPos.x + vScale.y / 2.f && vPos.x - vScale.y / 2.f < _Pos1.x - _Range);
			//bool c = (vPos.x + vScale.y / 2.f < _Pos1.x + _Range && _Pos1.x - _Range < vPos.x + vScale.y / 2.f);
			//bool d = (vPos.x - vScale.y / 2.f < _Pos1.x + _Range && _Pos1.x - _Range < vPos.x - vScale.y / 2.f);
			if ((vPos.y < _Pos1.y + _Range && _Pos1.y - _Range < vPos.y) 
				&& ((_Pos1.x + _Range < vPos.x + vScale.y / 2.f && vPos.x - vScale.y / 2.f < _Pos1.x - _Range)	// 벽이 영역을 지나감 또는
				|| (vPos.x + vScale.y / 2.f < _Pos1.x + _Range && _Pos1.x - _Range < vPos.x + vScale.y / 2.f)	// 벽의 최대값이 영역 내에 존재 또는
				|| (vPos.x - vScale.y / 2.f < _Pos1.x + _Range && _Pos1.x - _Range < vPos.x - vScale.y / 2.f))) // 벽의 최소값이 영역 내에 존재
			{
				vecWallList.push_back(vecList[i]);
			}
		}
	}

	return vecWallList;
}


vector<CGameObject*> FindDoorAround(Vec2 _Pos1, float _Range)
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	vector<CGameObject*> vecList;
	vector<CGameObject*> vecDoorList;

	vector<CGameObject*> vecObject = pCurLevel->GetLayer(1)->GetParentObjects();

	for (UINT i = 0; i < vecObject.size(); ++i)
	{
		if (vecObject[i]->Collider2D())
		{
			if (vecObject[i]->Collider2D()->GetName() == L"Door" )
				vecList.push_back(vecObject[i]);
		}
	}

	for (UINT i = 0; i < vecList.size(); ++i)
	{
		Vec3 vPos = vecList[i]->Transform()->GetRelativePos();
		Vec3 vScale = vecList[i]->Transform()->GetRelativeScale();
		Vec3 vRot = vecList[i]->Transform()->GetRelativeRotation();

		// 수직
		if (vRot.z == 0.f)
		{
			if ((vPos.y < _Pos1.y + _Range && _Pos1.y - _Range < vPos.y)
				&& ((_Pos1.x + _Range < vPos.x + vScale.y / 2.f && vPos.x - vScale.y / 2.f < _Pos1.x - _Range)	 
					|| (vPos.x + vScale.y / 2.f < _Pos1.x + _Range && _Pos1.x - _Range < vPos.x + vScale.y / 2.f)	
					|| (vPos.x - vScale.y / 2.f < _Pos1.x + _Range && _Pos1.x - _Range < vPos.x - vScale.y / 2.f)))
			{
				vecDoorList.push_back(vecList[i]);
			}
		}

		// 수평
		else
		{
			if ((vPos.x < _Pos1.x + _Range && _Pos1.x - _Range < vPos.x)
				&& ((_Pos1.y + _Range < vPos.y + vScale.y / 2.f && vPos.y - vScale.y / 2.f < _Pos1.y - _Range)
					|| (vPos.y + vScale.y / 2.f < _Pos1.y + _Range && _Pos1.y - _Range < vPos.y + vScale.y / 2.f)
					|| (vPos.y - vScale.y / 2.f < _Pos1.y + _Range && _Pos1.y - _Range < vPos.y - vScale.y / 2.f)))
			{
				vecDoorList.push_back(vecList[i]);
			}
		}
	}

	return vecDoorList;
}

Vec4 GetMaxMinPosWall(CGameObject* _Wall)
{
	Vec3 vPos = _Wall->Transform()->GetRelativePos();
	Vec3 vScale = _Wall->Transform()->GetRelativeScale();
	Vec3 vRot = _Wall->Transform()->GetRelativeRotation();
	Vec2 NewPosXY = Vec2(0.f, 0.f);

	float Theta = 0.f;
	float WallTheta = vRot.z * (180.f / XM_PI);

	float MaxWallPosX = 0.f;
	float MinWallPosX = 0.f;

	float MaxWallPosY = 0.f;
	float MinWallPosY = 0.f;

	// 수평
	if (WallTheta == 90.f)
	{
		MaxWallPosX = vPos.x + vScale.y / 2.f;
		MinWallPosX = vPos.x - vScale.y / 2.f;

		MaxWallPosY = vPos.y + vScale.x / 2.f;
		MinWallPosY = vPos.y - vScale.x / 2.f;
	}
	// 수직
	else
	{
		MaxWallPosX = vPos.x + vScale.x / 2.f;
		MinWallPosX = vPos.x - vScale.x / 2.f;

		MaxWallPosY = vPos.y + vScale.y / 2.f;
		MinWallPosY = vPos.y - vScale.y / 2.f;
	}
	return Vec4(MaxWallPosX, MaxWallPosY, MinWallPosX, MinWallPosY);
}

vector<tTileGridCell> MakeTileGrid()
{
	vector<tTileGridCell> vecGrid = {};
	CGameObject* TileObj =  CLevelMgr::GetInst()->FindObjectByName(L"TileMap");

	Vec2 ColRow = TileObj->TileMap()->GetTileColRow();

	int COL = ColRow.x;
	int ROW = ColRow.y;

	Vec2 TileSize = TileObj->TileMap()->GetTileSize();
	Vec3 TilePos = TileObj->Transform()->GetRelativePos();

	float TotalWidth = TileSize.x * COL;
	float TotalHeight = TileSize.y * ROW;

	Vec3 TopLeft = Vec3(TilePos.x - TotalWidth * 0.5f
		, TilePos.y + TotalHeight * 0.5f
		, TilePos.z);

	
	// 타일 마다 그리드를 생성해 vector에 집어넣는다.
	for (UINT j = 0; j < ROW; ++j)
	{
		Vec4 vColor = Vec4(0.f, 1.f, 0.f, 0.f);

		for (UINT i = 0; i < COL; ++i)
		{
			Vec3 RectPos = Vec3(TopLeft.x + TileSize.x * i + TileSize.x / 2.f
				, TopLeft.y - TileSize.y * (j + 1) + TileSize.y / 2.f
				, TilePos.z);

			tTileGridCell grid = {};
			grid.Position = Vec2(RectPos.x, RectPos.y);
			grid.Size = TileSize;

			grid.GridIndex = Vec2(i, j);
			if (IsWalkableTile(Vec2(RectPos.x, RectPos.y), TileSize))
				grid.Walkable = true;
			else
				grid.Walkable = false;

			vecGrid.push_back(grid);
			
		}
	}

	return vecGrid;
}

bool IsWalkableTile(Vec2 _Pos, Vec2 _Size)
{
	vector<CGameObject*> vecWallAround = FindWallAround(_Pos, _Size.x);

	for (UINT i = 0; i < vecWallAround.size(); ++i)
	{
		Vec3 WallRot = vecWallAround[i]->Transform()->GetRelativeRotation();
		Vec4 WallMaxMinPos = GetMaxMinPosWall(vecWallAround[i]);

		float WallMaxX = WallMaxMinPos.x;
		float WallMinX = WallMaxMinPos.z;

		float WallMaxY = WallMaxMinPos.y;
		float WallMinY = WallMaxMinPos.w;
		

		if ((_Pos.x - _Size.x / 2.f < WallMaxX && WallMaxX < _Pos.x + _Size.x / 2.f)	// 최대값 X가 내부
		|| (_Pos.x - _Size.x / 2.f < WallMinX && WallMinX < _Pos.x + _Size.x / 2.f))	// 최소값 X가 내부
		{
			// Y가 타일을 지나가고 있는지 확인한다.
			if ((_Pos.y - _Size.y / 2.f < WallMaxY && WallMaxY < _Pos.y + _Size.y / 2.f)
				|| (_Pos.y - _Size.y / 2.f < WallMinY && WallMinY < _Pos.y + _Size.y / 2.f) // Y의 최대값 혹은 최소값이 내부에 있다
				|| ((_Pos.y + _Size.y / 2.f < WallMaxY) && (WallMinY < _Pos.y - _Size.y / 2.f))) // Y의 최대값이 타일보다 크고, 최소값이 타일보다 작다. (타일을 관통중)
			{
				return false;
			}
		}
		
		bool a = (WallMaxY < _Pos.y - _Size.y / 2.f && WallMaxY < _Pos.y + _Size.y / 2.f);
		bool b = (WallMinY < _Pos.y - _Size.y / 2.f && WallMinY < _Pos.y + _Size.y / 2.f);

		if ((_Pos.y - _Size.y / 2.f < WallMaxY && WallMaxY < _Pos.y + _Size.y / 2.f)	// 최대값 Y가 내부
		|| (_Pos.y - _Size.y / 2.f < WallMinY && WallMinY < _Pos.y + _Size.y / 2.f))	// 최소값 Y가 내부
		{
			// X가 타일을 지나가고 있는지 확인한다.
			if ((_Pos.x - _Size.x / 2.f < WallMaxX && WallMaxX < _Pos.x + _Size.x / 2.f)
				|| (_Pos.x - _Size.x / 2.f < WallMinX && WallMinX < _Pos.x + _Size.x / 2.f) // X의 최대값 혹은 최소값이 내부에 있다
				|| ((_Pos.x + _Size.x / 2.f < WallMaxX) && (WallMinX < _Pos.x - _Size.x / 2.f))) // X의 최대값이 타일보다 크고, 최소값이 타일보다 작다. (타일을 관통중)
			{
				return false;
			}
		}
	}

	return true;
}
