#include "pch.h"
#include "CPrefab.h"

#include "CGameObject.h"


GAMEOBJECT_SAVE CPrefab::g_ObjectSave = nullptr;
GAMEOBJECT_LOAD CPrefab::g_ObjectLoad = nullptr;


CPrefab::CPrefab()
	: CAsset(ASSET_TYPE::PREFAB, false)
	, m_ProtoObj(nullptr)
{
}

CPrefab::CPrefab(const CPrefab& _Origin)
	: CAsset(_Origin)
	, m_ProtoObj(nullptr)
{
	if (nullptr != _Origin.m_ProtoObj)
	{
		m_ProtoObj = _Origin.m_ProtoObj->Clone();
	}
}

CPrefab::~CPrefab()
{
	if (nullptr != m_ProtoObj)
		delete m_ProtoObj;
}

void CPrefab::SetProtoObject(CGameObject* _Object)
{
	if (m_ProtoObj)
	{
		delete m_ProtoObj;
	}

	m_ProtoObj = _Object;
}

CGameObject* CPrefab::Instantiate()
{
	return m_ProtoObj->Clone();
}

int CPrefab::Save(const wstring& _FilePath)
{
	FILE* File = nullptr;
	_wfopen_s(&File, _FilePath.c_str(), L"wb");

	wstring key = GetKey();
	wstring path = GetRelativePath();

	SaveWString(key, File);
	SaveWString(path, File);

	bool Exist = m_ProtoObj;
	fwrite(&Exist, sizeof(bool), 1, File);

	if (m_ProtoObj)
	{
		g_ObjectSave(m_ProtoObj, File);
	}

	fclose(File);

	return S_OK;
}

int CPrefab::Load(const wstring& _FilePath)
{
	FILE* File = nullptr;
	_wfopen_s(&File, _FilePath.c_str(), L"rb");

	wstring key = GetKey();
	wstring path = GetRelativePath();

	LoadWString(key, File);
	LoadWString(path, File);

	SetKey(key);
	SetRelativePath(path);

	bool Exist = false;
	fread(&Exist, sizeof(bool), 1, File);

	if (Exist)
	{
		m_ProtoObj = g_ObjectLoad(File);
	}

	fclose(File);

	return S_OK;
}
