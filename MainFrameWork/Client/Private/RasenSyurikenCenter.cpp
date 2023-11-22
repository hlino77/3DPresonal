#include "stdafx.h"
#include "RasenSyurikenCenter.h"
#include "Client_Defines.h"
#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
#include "VIBuffer_Point.h"
#include "RigidBody.h"
#include "Pool.h"
#include "Player_Naruto.h"
#include "Rasengan_StartSmoke.h"
#include "Rasengan_RunSmoke.h"

CRasenSyurikenCenter::CRasenSyurikenCenter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CRasenSyurikenCenter::CRasenSyurikenCenter(const CRasenSyurikenCenter& rhs)
	: CEffect(rhs)
{
}

HRESULT CRasenSyurikenCenter::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CRasenSyurikenCenter::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	m_szModelName = L"Effect_RasenSyurikenCenter";

	m_bActive = false;


	m_vColor = Vec4(0.035f, 0.69f, 0.95f, 1.0f);
	m_vBlurColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);

	m_vBlurColor *= 0.2f;

	m_vTargetScale = Vec3(0.4f, 0.4f, 0.4f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Vec3(0.009f, 0.039f, 0.089f));

	return S_OK;
}

void CRasenSyurikenCenter::Tick(_float fTimeDelta)
{
	Update_Transform(fTimeDelta);

	BillBoard();
}

void CRasenSyurikenCenter::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CRasenSyurikenCenter::Render()
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


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vBlurColor", &m_vBlurColor, sizeof(Vec4))))
		return E_FAIL;


	if (FAILED(m_pBaseTexture->Set_SRV(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;


	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();


	Vec4 vWhite = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vWhite, sizeof(Vec4))))
		return E_FAIL;
	
	m_pRingTexture->Set_SRV(m_pShaderCom, "g_MaskTexture");
	m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture2");
	
	
	m_pShaderCom->Begin(1);
	
	m_pVIBufferCom->Render();


	Safe_Release(pGameInstance);
	return S_OK;
}


void CRasenSyurikenCenter::Appear()
{
	m_bActive = true;

	m_vCurrScale = Vec3(0.01f, 0.01f, 0.01f);
	m_pTransformCom->Set_Scale(Vec3(0.01f, 0.01f, 0.01f));

	BillBoard();
}

void CRasenSyurikenCenter::DisAppear()
{
	Set_Active(false);
}

void CRasenSyurikenCenter::BillBoard()
{
	Vec3 vCamPos = CGameInstance::GetInstance()->Get_CamPosition();

	m_pTransformCom->LookAt(vCamPos);
}


HRESULT CRasenSyurikenCenter::Ready_Components()
{
	__super::Ready_Components();



	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_RasenganCircle"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Ring"),
		TEXT("Com_RingTexture"), (CComponent**)&m_pRingTexture)))
		return E_FAIL;


	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Circle"),
		TEXT("Com_BaseTexture"), (CComponent**)&m_pBaseTexture)))
		return E_FAIL;


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Tex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;




	return S_OK;
}

void CRasenSyurikenCenter::Update_Transform(_float fTimeDelta)
{
	if (m_vCurrScale != m_vTargetScale)
	{
		m_vCurrScale += Vec3(1.0f, 1.0f, 1.0f) * 2.0f * fTimeDelta;
		if (m_vCurrScale.x > m_vTargetScale.x)
			m_vCurrScale = m_vTargetScale;
	}

	Matrix matOwnerWorld = m_pOwner->Get_TransformCom()->Get_WorldMatrix();

	m_pTransformCom->Set_WorldMatrix(matOwnerWorld);
	m_pTransformCom->Set_Scale(m_vCurrScale);

	BillBoard();
}


CGameObject* CRasenSyurikenCenter::Clone(void* pArg)
{
	CRasenSyurikenCenter* pInstance = new CRasenSyurikenCenter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CRasenSyurikenCenter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRasenSyurikenCenter* CRasenSyurikenCenter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRasenSyurikenCenter* pInstance = new CRasenSyurikenCenter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRasenSyurikenCenter");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CRasenSyurikenCenter::Free()
{
	__super::Free();
}
