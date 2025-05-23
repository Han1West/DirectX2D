#include "pch.h"
#include "CTransform.h"

#include "CDevice.h"
#include "CConstBuffer.h"

CTransform::CTransform()
	: CComponent(COMPONENT_TYPE::TRANSFORM)
	, m_IndependentScale(false)
{
	m_matWorld = XMMatrixIdentity();

}

CTransform::~CTransform()
{
}

void CTransform::FinalTick()
{
	m_matWorld = XMMatrixIdentity();

	Matrix matScale = XMMatrixScaling(m_RelativeScale.x, m_RelativeScale.y, m_RelativeScale.z);
	Matrix matRotation = XMMatrixRotationX(m_RelativeRotation.x) 
					   * XMMatrixRotationY(m_RelativeRotation.y) 
					   * XMMatrixRotationZ(m_RelativeRotation.z);
	Matrix matTrans = XMMatrixTranslation(m_RelativePos.x, m_RelativePos.y, m_RelativePos.z);
	
	// 크기 * 회전 * 이동
	m_matWorld = matScale * matRotation * matTrans;	

	// 방향벡터 갱신
	m_LocalDir[(UINT)DIR_TYPE::RIGHT]	= Vec3(1.f, 0.f, 0.f);
	m_LocalDir[(UINT)DIR_TYPE::UP]		= Vec3(0.f, 1.f, 0.f);
	m_LocalDir[(UINT)DIR_TYPE::FRONT]	= Vec3(0.f, 0.f, 1.f);

	for (int i = 0; i < 3; ++i)
	{			
		m_WorldDir[i] = m_LocalDir[i] = XMVector3TransformNormal(m_LocalDir[i], matRotation);
	}

	// 부모가 있다면
	if (GetOwner()->GetParent())
	{
		const Matrix& matParentWorld = GetOwner()->GetParent()->Transform()->GetWorldMat();

		// 자식 오브젝트는 부모 오브젝트의 크기에 영향을 받는다.
		if (false == m_IndependentScale)
		{
			m_matWorld *= matParentWorld;
		}

		// 자식 오브젝트가 독립적인 크기를 유지하고 싶은 경우
		else
		{
			Vec3 vParentWorldScale = GetOwner()->GetParent()->Transform()->GetWorldScale();
			Matrix matParentScaleInv = XMMatrixScaling(vParentWorldScale.x, vParentWorldScale.y, vParentWorldScale.z);
			matParentScaleInv = XMMatrixInverse(nullptr, matParentScaleInv);

			// 자식 LocalMat * 부모크기-1 * 부모월드행렬(부모크기 * 부모회전 * 부모이동)
			m_matWorld = m_matWorld * matParentScaleInv * matParentWorld;
		}
		

		// WorldDir 구하기
		m_WorldDir[(UINT)DIR_TYPE::RIGHT]	= Vec3(1.f, 0.f, 0.f);
		m_WorldDir[(UINT)DIR_TYPE::UP]		= Vec3(0.f, 1.f, 0.f);
		m_WorldDir[(UINT)DIR_TYPE::FRONT]	= Vec3(0.f, 0.f, 1.f);

		for (int i = 0; i < 3; ++i)
		{
			m_WorldDir[i] = XMVector3TransformNormal(m_WorldDir[i], m_matWorld);
			m_WorldDir[i].Normalize();
		}
	}
}

Vec3 CTransform::GetWorldScale()
{
	Vec3 vScale = m_RelativeScale;

	if (false == m_IndependentScale)
	{
		CGameObject* pParent = GetOwner()->GetParent();

		while (pParent)
		{
			vScale *= pParent->Transform()->GetRelativeScale();

			if (false == pParent->Transform()->m_IndependentScale)
				pParent = pParent->GetParent();
			else
				break;
		}
	}
	
	return vScale;
}

void CTransform::Binding()
{
	CConstBuffer* pCB = CDevice::GetInst()->GetCB(CB_TYPE::TRANSFORM);
		
	g_Trans.matWorld = m_matWorld;
	g_Trans.matWV = g_Trans.matWorld * g_Trans.matView;
	g_Trans.matWVP = g_Trans.matWV * g_Trans.matProj;


	pCB->SetData(&g_Trans);
	pCB->Binding();
}

void CTransform::SaveComponent(FILE* _File)
{
	fwrite(&m_RelativePos, sizeof(Vec3), 1, _File);
	fwrite(&m_RelativeScale, sizeof(Vec3), 1, _File);
	fwrite(&m_RelativeRotation, sizeof(Vec3), 1, _File);
	fwrite(&m_IndependentScale, sizeof(bool), 1, _File);
}

void CTransform::LoadComponent(FILE* _FILE)
{
	fread(&m_RelativePos, sizeof(Vec3), 1, _FILE);
	fread(&m_RelativeScale, sizeof(Vec3), 1, _FILE);
	fread(&m_RelativeRotation, sizeof(Vec3), 1, _FILE);
	fread(&m_IndependentScale, sizeof(bool), 1, _FILE);
}
