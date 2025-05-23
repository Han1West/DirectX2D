#include "pch.h"
#include "CMaterial.h"

#include "CDevice.h"
#include "CConstBuffer.h"



CMaterial::CMaterial(bool _EngineRes)
	: CAsset(ASSET_TYPE::MATERIAL, _EngineRes)
	, m_Const{}
{
	m_Const.v4Arr[0] = Vec4(1.f, 0.f, 0.f, 1.f);
}

CMaterial::CMaterial(const CMaterial& _Origin)
	: CAsset(_Origin)
	, m_Shader(_Origin.m_Shader)
	, m_Const(_Origin.m_Const)
	, m_arrTex{}
	, m_SharedMtrl(_Origin.m_SharedMtrl)	
{
	for(UINT i = 0; i < (UINT)TEX_PARAM::TEX_END; ++i)
	{
		m_arrTex[i] = _Origin.m_arrTex[i];
	}
}

CMaterial::~CMaterial()
{
}

void CMaterial::Binding()
{
	if (nullptr == m_Shader)
		return;

	// Texture 바인딩
	for (UINT i = 0; i < TEX_END; ++i)
	{
		if (nullptr == m_arrTex[i])
		{
			m_Const.bTex[i] = 0;
			CTexture::Clear(i);
			continue;
		}			

		m_Const.bTex[i] = 1;
		m_arrTex[i]->Binding(i);
	}

	// 상수 데이터 바인딩
	static CConstBuffer* pMtrlCB = CDevice::GetInst()->GetCB(CB_TYPE::MATERIAL);
	pMtrlCB->SetData(&m_Const);
	pMtrlCB->Binding();

	// Shader Binding
	m_Shader->Binding();
}

void CMaterial::SetTexParam(TEX_PARAM _Param, Ptr<CTexture> _tex)
{
	m_arrTex[_Param] = _tex;
}

CMaterial* CMaterial::Clone()
{
	CMaterial* pCloneMtrl = new CMaterial(*this);
	pCloneMtrl->m_SharedMtrl = this;
	return pCloneMtrl;
}

int CMaterial::Save(const wstring& _FilePath)
{
	// m_SharedMtrl 원형 재질을 가리킨다 == 동적재질이다.
	// 동적 재질을 저장하려고 했다. ==> 에러
	// 동적 재질 = 게임이 플레이되는 도중에 일시적으로 만들어서 쓰고 버리는 재질
	assert(!m_SharedMtrl.Get());

	FILE* pFile = nullptr;

	_wfopen_s(&pFile, _FilePath.c_str(), L"wb");
	assert(pFile);

	SaveAssetRef(m_Shader, pFile);

	fwrite(&m_Const, sizeof(MtrlConst), 1, pFile);

	for (int i = 0; i < (int)TEX_PARAM::TEX_END; ++i)
	{
		SaveAssetRef(m_arrTex[i], pFile);
	}

	fclose(pFile);

	return S_OK;
}


int CMaterial::Load(const wstring& _strFilePath)
{
	FILE* pFile = nullptr;

	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	if (nullptr == pFile)
		return E_FAIL;

	LoadAssetRef(m_Shader, pFile);

	fread(&m_Const, sizeof(MtrlConst), 1, pFile);

	for (int i = 0; i < (int)TEX_PARAM::TEX_END; ++i)
	{
		LoadAssetRef(m_arrTex[i], pFile);
	}

	fclose(pFile);

	return S_OK;
}
