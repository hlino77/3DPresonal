#include "stdafx.h"
#include "UI_BossIcon.h"
#include "GameInstance.h"

CUI_BossIcon::CUI_BossIcon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_BossIcon::CUI_BossIcon(const CGameObject & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_BossIcon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_BossIcon::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	__super::Initialize(pArg);


	BOSSICONDESC* pDesc = static_cast<BOSSICONDESC*>(pArg);
	m_iTextureIndex = pDesc->iTextureIndex;
	m_pBoss = pDesc->pBoss;


	m_fSizeX = 50.f * g_fSizeRatio;
	m_fSizeY = 50.f * g_fSizeRatio;
	m_fX = 354.8f * g_fSizeRatio;
	m_fY = 36.0f * g_fSizeRatio;;

	m_strObjectTag = L"UI_BossIcon";

	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, m_vUITargetPos.z));

	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);


	m_fMoveSpeed = 200.0f;

	m_fAlpha = 1.0f;

	Set_Active(false);

	return S_OK;
}

void CUI_BossIcon::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_BossIcon::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUI_BossIcon::Render()
{
	__super::Render();

	return S_OK;
}

void CUI_BossIcon::Appear()
{
	m_bActive = true;
	m_eState = UISTATE::TICK;
}

void CUI_BossIcon::UI_AppearTick(_float fTimeDelta)
{
}

void CUI_BossIcon::UI_Tick(_float fTimeDelta)
{



	if (m_pBoss->Is_Active() == false)
		Set_Active(false);
}

void CUI_BossIcon::UI_DisappearTick(_float fTimeDelta)
{

}



HRESULT CUI_BossIcon::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BossIcon"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_BossIcon::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return S_OK;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndex)))
		return E_FAIL;

	return S_OK;
}





CUI_BossIcon * CUI_BossIcon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_BossIcon*	pInstance = new CUI_BossIcon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_BossIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_BossIcon::Clone(void* pArg)
{
	CUI_BossIcon*	pInstance = new CUI_BossIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_BossIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_BossIcon::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
