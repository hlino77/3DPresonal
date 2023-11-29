#include "stdafx.h"
#include "WireTrail.h"
#include "Client_Defines.h"
#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
#include "VIBuffer_Point.h"
#include "RigidBody.h"
#include "Player.h"
#include "Pool.h"

CWireTrail::CWireTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CWireTrail::CWireTrail(const CWireTrail& rhs)
	: CEffect(rhs)
{
}

HRESULT CWireTrail::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CWireTrail::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	m_pRigidBody->UseGravity(false);
	m_pRigidBody->SetCompareGruond(false);

	m_szModelName = L"Effect_WireTrail";

	m_bActive = false;

	Ready_InstanceBuffer();


	WIRETRAIL* pDesc = static_cast<WIRETRAIL*>(pArg);

	m_pOwner = pDesc->pOwner;
	m_iBoneIndex = pDesc->iBoneIndex;


	m_vColor = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_vBlurColor = Vec4(0.074f, 0.24f, 0.58f, 1.0f);

	return S_OK;
}

void CWireTrail::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bDisappear)
	{
		Matrix matWorld = m_pOwner->Get_ModelCom()->Get_CurrBoneMatrix(m_iBoneIndex);
		matWorld *= m_pOwner->Get_TransformCom()->Get_WorldMatrix();
		Vec3 vHandPos(matWorld.m[3]);
		Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_float fLength = (vHandPos - vPos).Length();
		if(fLength < 0.1f)
			Set_Active(false);
		else
		{
			vPos = Vec3::Lerp(vPos, vHandPos, 15.0f * fTimeDelta);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
			Update_Point();
		}
	}
	else
	{
		Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos = Vec3::Lerp(vPos, m_vTargetPos, 15.0f * fTimeDelta);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		Update_Point();
	}
}

void CWireTrail::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CWireTrail::Render()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	D3D11_MAPPED_SUBRESOURCE		SubResource = {};

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, &matTrailData, sizeof(Matrix));

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


	m_pVIBufferCom->Render_Instance(m_pInstanceBuffer, 1);


	Safe_Release(pGameInstance);
	return S_OK;
}


void CWireTrail::Appear()
{
	Set_Active(true);
	m_bDisappear = false;
	m_vColor.w = 1.0f;

	ZeroMemory(&matTrailData, sizeof(Matrix));

	Matrix matWorld = m_pOwner->Get_ModelCom()->Get_CurrBoneMatrix(m_iBoneIndex);
	matWorld *= m_pOwner->Get_TransformCom()->Get_WorldMatrix();
	Vec3 vPos(matWorld.m[3]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	Vec3 vDir = m_pOwner->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	vDir.Normalize();
	m_pTransformCom->LookAt(vPos + vDir);

	m_vTargetPos = vPos + vDir * 20.0f;
}

void CWireTrail::Disappear()
{
	m_bDisappear = true;
}

void CWireTrail::Update_Point()
{
	Matrix matWorld = m_pOwner->Get_ModelCom()->Get_CurrBoneMatrix(m_iBoneIndex);
	matWorld *= m_pOwner->Get_TransformCom()->Get_WorldMatrix();
	Vec3 vHandPos(matWorld.m[3]);
	Vec3 vCamPos = CGameInstance::GetInstance()->Get_CamPosition();
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vec3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);


	Vec3 vCamDir = vCamPos - vPos;
	vCamDir.Normalize();

	Vec3 vRight = vCamDir.Cross(vLook);
	vRight.Normalize();

	vRight *= 0.005f;

	Vec3 vCurrRightPos = vPos + vRight;
	Vec3 vCurrLeftPos = vPos - vRight;
	Vec3 vPrevRightPos = vHandPos + vRight;
	Vec3 vPrevLeftPos = vHandPos - vRight;

	matTrailData.Right(vCurrRightPos);
	matTrailData.Up(vCurrLeftPos);
	matTrailData.Backward(vPrevRightPos);
	matTrailData.Translation(vPrevLeftPos);
}


HRESULT CWireTrail::Ready_Components()
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

HRESULT CWireTrail::Ready_InstanceBuffer()
{
	D3D11_BUFFER_DESC			BufferDesc;

	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));


	// m_BufferDesc.ByteWidth = 정점하나의 크기(Byte) * 정점의 갯수;
	BufferDesc.ByteWidth = sizeof(VTXINSTANCE);
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* 정적버퍼로 할당한다. (Lock, unLock 호출 불가)*/
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferDesc.MiscFlags = 0;
	BufferDesc.StructureByteStride = sizeof(VTXINSTANCE);

	D3D11_SUBRESOURCE_DATA		InitialData;

	vector<Matrix> InitMatrix;

	InitMatrix.resize(1, XMMatrixIdentity());

	InitialData.pSysMem = InitMatrix.data();

	if (FAILED(m_pDevice->CreateBuffer(&BufferDesc, &InitialData, &m_pInstanceBuffer)))
		return E_FAIL;


	return S_OK;
}

CGameObject* CWireTrail::Clone(void* pArg)
{
	CWireTrail* pInstance = new CWireTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CFootTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CWireTrail* CWireTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWireTrail* pInstance = new CWireTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFootTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CWireTrail::Free()
{
	__super::Free();
}
