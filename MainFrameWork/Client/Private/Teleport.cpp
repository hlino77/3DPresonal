#include "stdafx.h"
#include "Teleport.h"
#include "Client_Defines.h"
#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
#include "VIBuffer_Rect.h"
#include "RigidBody.h"
#include "Pool.h"

CTeleport::CTeleport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CTeleport::CTeleport(const CTeleport& rhs)
	: CEffect(rhs)
{
}

HRESULT CTeleport::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CTeleport::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	m_szModelName = L"Effect_Teleport";

	m_bActive = false;

	m_vColor = Vec4(0.0f, 0.0f, 0.0f, 0.7f);

	m_pTransformCom->Set_Scale(Vec3(0.8f, 0.2f, 1.0f));

	m_vTargetScale = Vec3(0.8f, 2.0f, 1.0f);

	m_fAlphaWeight = 1.0f;

	m_fEffectTime = 0.1f;

	return S_OK;
}

void CTeleport::Tick(_float fTimeDelta)
{
	m_fCurrTime += fTimeDelta;
	if (m_fCurrTime >= m_fEffectTime)
		m_bEnd = true;


	Vec3 vScale = m_pTransformCom->Get_Scale();
	vScale = Vec3::Lerp(vScale, m_vTargetScale, 20.0f * fTimeDelta);
	m_pTransformCom->Set_Scale(vScale);

	BillBoard();

	if (m_bEnd)
	{
		Set_Active(false);
		CPool<CTeleport>::Return_Obj(this);
	}

}

void CTeleport::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CTeleport::Render()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(Vec4))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;

	if (FAILED(m_pLineTexture->Set_SRV(m_pShaderCom, "g_MaskTexture2")))
		return E_FAIL;



	m_pShaderCom->Begin(4);

	m_pVIBufferCom->Render();


	Safe_Release(pGameInstance);
	return S_OK;
}


void CTeleport::Appear(Vec3 vPos, Vec3 vUp)
{
	m_bActive = true;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->Set_Up(vUp);

	m_pTransformCom->Set_Scale(Vec3(0.8f, 0.2f, 1.0f));

	m_fCurrTime = 0.0f;

	m_bEnd = false;
}


HRESULT CTeleport::Ready_Components()
{
	__super::Ready_Components();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Tex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Circle"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_TeleportLine"),
		TEXT("Com_LineTexture"), (CComponent**)&m_pLineTexture)))
		return E_FAIL;


	return S_OK;
}

void CTeleport::BillBoard()
{
	Vec3 vCamPos = CGameInstance::GetInstance()->Get_CamPosition();

	m_pTransformCom->LookAt_SetUp(vCamPos);
}

CGameObject* CTeleport::Clone(void* pArg)
{
	CTeleport* pInstance = new CTeleport(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTeleport");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CTeleport* CTeleport::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTeleport* pInstance = new CTeleport(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTeleport");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CTeleport::Free()
{
	__super::Free();
}
