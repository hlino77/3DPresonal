#include "stdafx.h"
#include "FootTrail.h"
#include "Client_Defines.h"
#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
#include "VIBuffer_Point.h"
#include "RigidBody.h"
#include "Player.h"
#include "Pool.h"

CFootTrail::CFootTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CFootTrail::CFootTrail(const CFootTrail& rhs)
	: CEffect(rhs)
{
}

HRESULT CFootTrail::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CFootTrail::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	m_pRigidBody->UseGravity(false);
	m_pRigidBody->SetCompareGruond(false);

	m_szModelName = L"Effect_FootTrail";

	m_bActive = false;

	m_iMaxSize = 100;

	Ready_InstanceBuffer();

	return S_OK;
}

void CFootTrail::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bDisappear)
	{
		m_vColor.w -= 1.f * fTimeDelta;
		if (m_vColor.w <= 0.0f)
		{
			m_vColor.w = 0.0f;
			Set_Active(false);
			CPool<CFootTrail>::Return_Obj(this);
		}
	}
	else
	{
		Add_Point();
	}
}

void CFootTrail::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CFootTrail::Render()
{
	if (m_PointList.size() <= 1)
		return S_OK;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	vector<Matrix> WorldMatrix;
	WorldMatrix.reserve(m_PointList.size());

	for (auto& Point : m_PointList)
	{
		WorldMatrix.push_back(Point.matPointData);
	}

	D3D11_MAPPED_SUBRESOURCE		SubResource = {};

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, WorldMatrix.data(), sizeof(Matrix) * WorldMatrix.size());

	m_pContext->Unmap(m_pInstanceBuffer, 0);



	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &Vec3(pGameInstance->Get_CamPosition()), sizeof(Vec3))))
		return E_FAIL;


	/*Vec4 vEmissive(1.0f, 1.0f, 1.0f, 0.0f);

	vEmissive *= 0.8f;*/

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vEmissive", &vEmissive, sizeof(Vec4))))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(Vec4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vBlurColor", &m_vBlurColor, sizeof(Vec4))))
		return E_FAIL;


	m_pShaderCom->Begin(0);


	m_pVIBufferCom->Render_Instance(m_pInstanceBuffer, m_PointList.size());


	Safe_Release(pGameInstance);
	return S_OK;
}


void CFootTrail::Appear(CGameObject* pOwner, _uint iBoneIndex, Vec4 vColor, Vec4 vBlurColor)
{
	m_pOwner = pOwner;
	m_iBoneIndex = iBoneIndex;
	m_vColor = vColor;
	m_vBlurColor = vBlurColor;

	m_PointList.clear();
	Set_Active(true);
	m_bDisappear = false;
	m_vColor.w = 1.0f;
}

void CFootTrail::Disappear()
{
	m_bDisappear = true;
}


HRESULT CFootTrail::Ready_Components()
{
	__super::Ready_Components();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Trail_Instance"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CFootTrail::Ready_InstanceBuffer()
{
	D3D11_BUFFER_DESC			BufferDesc;

	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));


	// m_BufferDesc.ByteWidth = 정점하나의 크기(Byte) * 정점의 갯수;
	BufferDesc.ByteWidth = sizeof(VTXINSTANCE) * m_iMaxSize;
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* 정적버퍼로 할당한다. (Lock, unLock 호출 불가)*/
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = sizeof(VTXINSTANCE);

	D3D11_SUBRESOURCE_DATA		InitialData;

	vector<Matrix> InitMatrix;

	InitMatrix.resize(m_iMaxSize, XMMatrixIdentity());

	InitialData.pSysMem = InitMatrix.data();

	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &InitialData, &m_pInstanceBuffer)))
		return E_FAIL;


	return S_OK;
}

void CFootTrail::Add_Point()
{
	Matrix matWorld = m_pOwner->Get_ModelCom()->Get_CurrBoneMatrix(m_iBoneIndex);
	matWorld *= m_pOwner->Get_TransformCom()->Get_WorldMatrix();

	Vec3 vStartPos(matWorld.m[3]);

	TRAILPOINT tCurr;
	
	Matrix matData = XMMatrixIdentity();

	if (m_PointList.empty())
	{
		matData.Right(vStartPos);
		matData.Up(vStartPos);
		matData.Backward(vStartPos);
		matData.Translation(vStartPos);
	}
	else
	{
		TRAILPOINT& tPrev = m_PointList.front();

		/*if ((vStartPos - tPrev.vPoint).Length() < 0.01f)
		{
			dynamic_cast<CPlayer*>(m_pOwner)->DisAppear_FootTrail();
			return;
		}*/
		


		Vec3 vCamPos = CGameInstance::GetInstance()->Get_CamPosition();
		Vec3 vLook = vCamPos - vStartPos;

		Vec3 vPrevDir = tPrev.vPoint - vStartPos;

		Vec3 vRight = vPrevDir.Cross(vLook);
		vRight.Normalize();


		Vec3 vCurrRightPos = vStartPos + vRight * 0.008f;
		Vec3 vCurrLeftPos = vStartPos - vRight * 0.008f;



		Vec3 vPrevRightPos = tPrev.matPointData.Right();
		Vec3 vPrevLeftPos = tPrev.matPointData.Up();

		matData.Right(vCurrRightPos);
		matData.Up(vCurrLeftPos);
		matData.Backward(vPrevRightPos);
		matData.Translation(vPrevLeftPos);
	}

	tCurr.vPoint = vStartPos;
	tCurr.matPointData = matData;
	m_PointList.push_front(tCurr);


	_int iOverCount = m_PointList.size() - m_iMaxSize;
	if (iOverCount > 0)
	{
		for (_uint i = 0; i < iOverCount; ++i)
			m_PointList.pop_back();
	}
}


CGameObject* CFootTrail::Clone(void* pArg)
{
	CFootTrail* pInstance = new CFootTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CFootTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CFootTrail* CFootTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFootTrail* pInstance = new CFootTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFootTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CFootTrail::Free()
{
	__super::Free();
}
