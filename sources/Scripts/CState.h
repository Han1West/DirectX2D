#pragma once

#include <Engine/CEntity.h>
#include "CStateMgr.h"
#include <Engine/CScript.h>
#include <cmath>

struct AABB
{
	Vec2 Min;
	Vec2 Max;

	AABB(Vec2 _min, Vec2 _max) 
		: Min(_min), Max(_max)
	{
	}
};


typedef struct _tagNode
{
	Vec2	vPos;

	_tagNode* pPrevNode;   // ���� ���
	float		fFromParent; // ���� ��忡�� ���� �������� �Ÿ�
	float		fToDest;	 // ���� ��忡�� ������ ������ �Ÿ�
	float		fFinal;		 // ���� �� ���� ��ģ ��(�켱���� ����)

	int			iIdxX;
	int			iIdxY;
	bool		bMove;   // �̵� ���� �Ұ���
	bool		bOpen;   // OpenList �� ������
	bool		bClosed; // ClostList �� ����ִ���

	bool		bPath;
}tNode;

// Open List
class Compare
{
public:
	bool operator() (tNode* _pLeft, tNode* _pRight)
	{
		return _pLeft->fFinal > _pRight->fFinal;
	}
};

class CScript;

class CState
	: public CEntity
{
public:
	CScript* m_Owner;

	STATE_TYPE m_StateType;

	priority_queue<tNode*, vector<tNode*>, Compare>	m_OpenList;
	vector<vector<tNode>> m_vecNode;

	int m_iXCount;
	int m_iYCount;

	int m_iCurX;
	int m_iCurY;
	int m_iDestX;
	int m_iDestY;

public:
	virtual void Enter() {};
	virtual void FinalTick() {};
	virtual void Exit() {};

	void SetOwner(CScript* _Script) { m_Owner = _Script; }
	CScript* GetOwner() { return m_Owner; }
	STATE_TYPE GetStateType() { return m_StateType; }

public:
	float GetThetaWithPlayer();
	//Vec4 GetMaxMinPosWall(CGameObject* _Wall);
	bool LineSegmentsIntersect(const Vec2& Player, const Vec2& Object, const Vec2& WallEdge1, const Vec2& WallEdge2);
	bool IsPointOnSegment(const Vec2& p, const Vec2& a, const Vec2& b);
	bool FindWall(Vec3 _Pos1, Vec3 _Pos2);
	
	void MoveToward(CGameObject* _Obj, Vec2 _DesPos, float _Speed);


	CGameObject* FindWeaponAround(Vec3 _EnemyPos);

public:
	void Rebuild(priority_queue<tNode*, vector<tNode*>, Compare>& _queue);
	void CalculateCost(tNode* _pCurNode, tNode* _pOrigin);
	void AddOpenList(int _iXIdx, int _iYIdx, tNode* _pOrigin);

	list<Vec2> FindPath(Vec3 _CurPos, Vec3 _Dest);

public:
	CLONE(CState);
	CState(STATE_TYPE _Type);
	virtual ~CState();

};

