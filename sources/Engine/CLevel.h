#pragma once

#include "CEntity.h"

#include "CLayer.h"

class CLevel
	: public CEntity
{
public:
	static LEVEL_SAVE g_LevelSave;
	static LEVEL_LOAD g_LevelLoad;

private:
	wstring		m_RelativePath;
	LEVEL_STATE	m_State;
	CLayer		m_arrLayer[MAX_LAYER];
		
public:
	CLayer* GetLayer(int _LayerIdx) { return &m_arrLayer[_LayerIdx]; }
	CGameObject* FindObjectByName(const wstring& _Name);		
	LEVEL_STATE GetState() { return m_State; }

	void RegisterClear()
	{
		// 이전프레임에 등록받았던 오브젝트 목록을 Clear 한다.
		for (UINT i = 0; i < MAX_LAYER; ++i)
		{
			m_arrLayer[i].m_vecObjects.clear();
		}
	}

public:
	void Begin();
	void Tick();
	void FinalTick();	

public:
	void AddObject(int _LayerIdx, CGameObject* _Object, bool _MoveWithChild);

private:
	void ChangeState(LEVEL_STATE _NextState);

public:
	void SetRelativePath(wstring _Path) { m_RelativePath = _Path; }

	wstring GetRelativePath() { return m_RelativePath; }

public:
	void SaveRelativePath(FILE* _File);
	void LoadRelativePath(FILE* _File);

public:
	virtual CLevel* Clone() { return new CLevel(*this); }
	CLevel();
	~CLevel();

	friend class CLevelMgr;
};

