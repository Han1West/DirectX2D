#include "pch.h"
#include "CState.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>

#include "CPlayerScript.h"


CState::CState(STATE_TYPE _Type)
	: m_StateType(_Type)
	, m_Owner(nullptr)
	, m_iXCount(0)
	, m_iYCount	(0)
	, m_iCurX(0)
	, m_iCurY(0)
	, m_iDestX(0)
	, m_iDestY(0)
{
}

CState::~CState()
{
}

float CState::GetThetaWithPlayer()
{
	CGameObject* pPlayer = CLevelMgr::GetInst()->FindObjectByName(L"Player");
	Vec3 PlayerPos = pPlayer->Transform()->GetRelativePos();
	Vec3 OwnerPos = m_Owner->GetOwner()->Transform()->GetRelativePos();

	Vec2 Dir = Vec2(OwnerPos.x - PlayerPos.x, OwnerPos.y - PlayerPos.y);
	Dir.Normalize();

	return atan2(Dir.y, Dir.x);
}

bool CState::LineSegmentsIntersect(const Vec2& Player, const Vec2& Object, const Vec2& WallEdge1, const Vec2& WallEdge2)
{
	// �� C �� a , b�������� �����̸� true
	auto ccw = [](const Vec2& a, const Vec2& b, const Vec2& c) 
	{
		return (c.y - a.y) * (b.x - a.x) > (b.y - a.y) * (c.x - a.x);
	};

	bool PlayerIsLeft = ccw(Player, WallEdge1, WallEdge2);
	bool ObjectIsLeft = ccw(Object, WallEdge1, WallEdge2);

	bool Wall1LeftBoth = ccw(Player, Object, WallEdge1);
	bool Wall2LeftBoth = ccw(Player, Object, WallEdge2);

	if ((ccw(Player, WallEdge1, WallEdge2) != ccw(Object, WallEdge1, WallEdge2)) && (ccw(Player, Object, WallEdge1) != ccw(Player, Object, WallEdge2)))
			return true;

	bool A = IsPointOnSegment(Player, WallEdge1, WallEdge2);
	bool B = IsPointOnSegment(Object, WallEdge1, WallEdge2);
	bool C = IsPointOnSegment(Player, WallEdge1, WallEdge2);
	bool D = IsPointOnSegment(Object, WallEdge1, WallEdge2);

	return IsPointOnSegment(Player, WallEdge1, WallEdge2) || IsPointOnSegment(Object, WallEdge1, WallEdge2) ||
		IsPointOnSegment(Player, WallEdge1, WallEdge2) || IsPointOnSegment(Object, WallEdge1, WallEdge2);
}

bool CState::IsPointOnSegment(const Vec2& p, const Vec2& a, const Vec2& b)
{
	// �� ���� ������ ���� �ִ��� Ȯ��
	float cross = (b.y - a.y) * (p.x - a.x) - (b.x - a.x) * (p.y - a.y);
	if (fabs(cross) > std::numeric_limits<float>::epsilon()) {
		return false; 
	}

	// 2. p�� a�� b ���̿� �ִ��� Ȯ�� 
	return min(a.x, b.x) <= p.x && p.x <= max(a.x, b.x) &&
		min(a.y, b.y) <= p.y && p.y <= max(a.y, b.y);
}

bool CState::FindWall(Vec3 _Pos1, Vec3 _Pos2)
{
	// �� ������Ʈ ������ ������ ��� ���Ѵ�.
	vector<CGameObject*> vecWallObj = FindWallBetween(Vec2(_Pos1.x, _Pos1.y), Vec2(_Pos2.x, _Pos2.y));

	// ���̿� ���� ����.
	if (vecWallObj.size() == 0)
		return false;


	// ������ ���� ���� �������� �׸��� �����ϴ��� �����Ѵ�.
	for (UINT i = 0; i < vecWallObj.size(); ++i)
	{

		Vec4 WallMaxMin = GetMaxMinPosWall(vecWallObj[i]);

		Vec2 vWallMax = Vec2(WallMaxMin.x + 10.f, WallMaxMin.y + 10.f);
		Vec2 vWallMin = Vec2(WallMaxMin.z - 10.f, WallMaxMin.w - 10.f);
		AABB Wall(vWallMin, vWallMax);

 		if (vecWallObj[i]->GetName() == L"Door")
		{
			vWallMax = Vec2(WallMaxMin.x , WallMaxMin.y );
			vWallMin = Vec2(WallMaxMin.z , WallMaxMin.w );
		}

		vector<pair<Vec2, Vec2>> edges =
		{
		{Wall.Min, {Wall.Max.x, Wall.Min.y}}, // �ϴ�
		{Wall.Min, {Wall.Min.x, Wall.Max.y}}, // ����
		{{Wall.Min.x, Wall.Max.y}, Wall.Max}, // ���
		{{Wall.Max.x, Wall.Min.y}, Wall.Max}  // ������
		};


		for (const auto& edge : edges)
		{
			if (LineSegmentsIntersect(Vec2(_Pos1.x, _Pos1.y), Vec2(_Pos2.x, _Pos2.y), edge.first, edge.second))
			{
				// �ϳ��� ���̶� �ɸ��� true
				return true;
			}
		}
	}

	// �ɸ��� ���� ���ٸ� false
	return false;
}

void CState::MoveToward(CGameObject* _Obj, Vec2 _DesPos, float _Speed)
{
	Vec3 vCurPos = _Obj->Transform()->GetRelativePos();
	Vec3 vCurRot = _Obj->Transform()->GetRelativeRotation();

	Vec2 Dir = Vec2(_DesPos.x - vCurPos.x, _DesPos.y - vCurPos.y);
	Dir.Normalize();

	vCurPos.x += _Speed * Dir.x * DT;
	vCurPos.y += _Speed * Dir.y * DT;

	vCurRot.z = atan2(Dir.y, Dir.x);

	_Obj->Transform()->SetRelativePos(vCurPos);
	_Obj->Transform()->SetRelativeRotation(vCurRot);
}

CGameObject* CState::FindWeaponAround(Vec3 _EnemyPos)
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurrentLevel();
	vector<CGameObject*> vecList;
	vector<CGameObject*> vecWallList;
	float MinDistance = 500.f;
	int MinWeaponIdx = -1;

	vector<CGameObject*> vecWeapon = pCurLevel->GetLayer(4)->GetParentObjects();

	for (UINT i = 0; i < vecWeapon.size(); ++i)
	{
		Vec3 WeaponPos = vecWeapon[i]->Transform()->GetRelativePos();

		// ���� ���� �� �� ���� ����� Skip
		if (vecWeapon[i]->GetName() == L"Katana" || vecWeapon[i]->GetName() == L"Axe"
			|| vecWeapon[i]->GetName() == L"Pistol" || vecWeapon[i]->GetName() == L"SMG")
			continue;

		// ���� ���� Ư�� �Ÿ��� ������� ã�´�.
		if (!FindWall(WeaponPos, _EnemyPos))
		{
			float Distance = WeaponPos.Distance(WeaponPos, _EnemyPos);
			// ���� ����� ���⸦ ã�´�.
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				MinWeaponIdx = i;
			}
		}
	}

	if (MinWeaponIdx == -1)
		return nullptr;

	return vecWeapon[MinWeaponIdx];
}

void CState::Rebuild(priority_queue<tNode*, vector<tNode*>, Compare>& _queue)
{
	priority_queue<tNode*, vector<tNode*>, Compare> tempQueue;

	while (!_queue.empty())
	{
		tempQueue.push(_queue.top());
		_queue.pop();
	}

	_queue.swap(tempQueue);

}

void CState::CalculateCost(tNode* _pCurNode, tNode* _pOrigin)
{
	_pCurNode->pPrevNode = _pOrigin;
	_pCurNode->fFromParent = 10.f + _pOrigin->fFromParent;
	_pCurNode->fToDest = abs(m_iDestX - _pCurNode->iIdxX) * 10.f + abs(m_iDestY - _pCurNode->iIdxY) * 10;
	_pCurNode->fFinal = _pCurNode->fFromParent + _pCurNode->fToDest;
}

void CState::AddOpenList(int _iXIdx, int _iYIdx, tNode* _pOrigin)
{
	// ���� �������� �� �� �ִ� ���� OpenList �� �ִ´�.
	// ��� ������ ��� ���
	if (_iXIdx < 0 || _iXIdx >= m_iXCount || _iYIdx < 0 || _iYIdx >= m_iYCount
		|| !m_vecNode[_iXIdx][_iYIdx].bMove)
		return;

	// �ش� ���� Clost List �� �ִ� ���, Open List �� ���� �ʴ´�.
	if (m_vecNode[_iXIdx][_iYIdx].bClosed)
		return;

	// Open List �� ����� ����ؼ� �ִ´�.
	if (false == m_vecNode[_iXIdx][_iYIdx].bOpen)
	{
		CalculateCost(&m_vecNode[_iXIdx][_iYIdx], _pOrigin);
		// Open List �� �ִ´�.
		m_vecNode[_iXIdx][_iYIdx].bOpen = true;
		m_OpenList.push(&m_vecNode[_iXIdx][_iYIdx]);
	}
	else // �̹� OpenList �� �ִ� ���,
	{
		//����� ����ؼ� �� ȿ���� ���� ������ ��ü�Ѵ�.
		tNode node = m_vecNode[_iXIdx][_iYIdx];
		CalculateCost(&node, _pOrigin);

		if (m_vecNode[_iXIdx][_iYIdx].fFinal > node.fFinal)
		{
			m_vecNode[_iXIdx][_iYIdx] = node;

			// ���¸���Ʈ(�켱����ť) �缳��
			Rebuild(m_OpenList);
		}
	}
}


list<Vec2> CState::FindPath(Vec3 _CurPos, Vec3 _Dest)
{
	if (_CurPos.Distance(_CurPos, _Dest) < 32.f)
		return list<Vec2>();

	CGameObject* pPlayer = CLevelMgr::GetInst()->FindObjectByName(L"Player");
	CPlayerScript* pPlayerScript = (CPlayerScript*)pPlayer->GetScripts()[0];
	
	vector<tTileGridCell> vecTileGrid = pPlayerScript->GetTileGrid();

	CGameObject* TileObj = CLevelMgr::GetInst()->FindObjectByName(L"TileMap");
	Vec2 ColRow = TileObj->TileMap()->GetTileColRow();
	m_iXCount = (int)ColRow.x;
	m_iYCount = (int)ColRow.y;

	m_vecNode.resize((int)ColRow.x, vector<tNode>((int)ColRow.y));
	
	for (UINT i = 0; i < vecTileGrid.size(); ++i)
	{
		tNode NewNode = {};

		// ���� ��� ����
		Vec2 GridPos = vecTileGrid[i].Position;
		Vec2 GridSize = vecTileGrid[i].Size;

		NewNode.vPos = vecTileGrid[i].Position;
		NewNode.iIdxX = vecTileGrid[i].GridIndex.x;
		NewNode.iIdxY = vecTileGrid[i].GridIndex.y;
		NewNode.bMove = vecTileGrid[i].Walkable;

		if (GridPos.x - GridSize.x / 2.f < _CurPos.x && _CurPos.x < GridPos.x + GridSize.x / 2.f
			&& GridPos.y - GridSize.y / 2.f < _CurPos.y && _CurPos.y < GridPos.y + GridSize.y / 2.f)
		{
			m_iCurX = (int)vecTileGrid[i].GridIndex.x;
			m_iCurY = (int)vecTileGrid[i].GridIndex.y;
			NewNode.bClosed = true;
			NewNode.bMove = true;
		}

		if (GridPos.x - GridSize.x / 2.f < _Dest.x && _Dest.x < GridPos.x + GridSize.x / 2.f
			&& GridPos.y - GridSize.y / 2.f < _Dest.y && _Dest.y < GridPos.y + GridSize.y / 2.f)
		{
			m_iDestX = (int)vecTileGrid[i].GridIndex.x;
			m_iDestY = (int)vecTileGrid[i].GridIndex.y;
			NewNode.bMove = true;
		}


		int row = i / (int)ColRow.x; // 2���� ������ ��
		int col = i % (int)ColRow.x; // 2���� ������ ��

		m_vecNode[col][row] = NewNode;
	}

	tNode* pCurNode = &m_vecNode[m_iCurX][m_iCurY];

	while (true)
	{
		// ���� �������� 4������ ��带 OpenList �� �ִ´�.
		// UP
		AddOpenList(pCurNode->iIdxX
			, pCurNode->iIdxY - 1, pCurNode);

		// RIGHT		
		AddOpenList(pCurNode->iIdxX + 1
			, pCurNode->iIdxY, pCurNode);

		// DOWN		
		AddOpenList(pCurNode->iIdxX
			, pCurNode->iIdxY + 1, pCurNode);

		// LEFT		
		AddOpenList(pCurNode->iIdxX - 1
			, pCurNode->iIdxY, pCurNode);

		// ��
		AddOpenList(pCurNode->iIdxX + 1
			, pCurNode->iIdxY - 1, pCurNode);

		// ��
		AddOpenList(pCurNode->iIdxX - 1
			, pCurNode->iIdxY - 1, pCurNode);

		// ��
		AddOpenList(pCurNode->iIdxX + 1
			, pCurNode->iIdxY + 1, pCurNode);

		// ��
		AddOpenList(pCurNode->iIdxX - 1
			, pCurNode->iIdxY + 1, pCurNode);

		// 2. Open List ���� ���� ȿ���� ���� ��带 ������.
		//  - �ش� ���� ClostList �� �ִ´�.
		if (m_OpenList.size() == 0)
			return list<Vec2>();
		
		pCurNode = m_OpenList.top();
		
		pCurNode->bClosed = true;

		m_OpenList.pop();

		// �������� ������ ��� Astar ����
		if (pCurNode->iIdxX == m_iDestX && pCurNode->iIdxY == m_iDestY)
		{
			break;
		}
	}

	list<Vec2> listPath = {};

	while (true)
	{
		m_vecNode[pCurNode->iIdxX][pCurNode->iIdxY].bPath = true;
		listPath.push_front(pCurNode->vPos);
		pCurNode = pCurNode->pPrevNode;
	
		if (pCurNode->iIdxX == m_iCurX && pCurNode->iIdxY == m_iCurY)
		{
			pCurNode->bPath = true;
			listPath.push_front(pCurNode->vPos);
			break;
		}
	}

	return listPath;
}

