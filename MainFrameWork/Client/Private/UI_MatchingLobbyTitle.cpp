#include "stdafx.h"
#include "UI_MatchingLobbyTitle.h"
#include "GameInstance.h"

CUI_MatchingLobbyTitle::CUI_MatchingLobbyTitle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_MatchingLobbyTitle::CUI_MatchingLobbyTitle(const CGameObject & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_MatchingLobbyTitle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_MatchingLobbyTitle::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	__super::Initialize(pArg);

	m_fSizeX = 900 * 0.5f * g_fSizeRatio;
	m_fSizeY = 200 * 0.5f * g_fSizeRatio;
	m_fX = 306.4f * g_fSizeRatio;
	m_fY = 53.0f * g_fSizeRatio;

	m_strObjectTag = L"MatchingLobbyTitle";

	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.0f));

	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);
	
	m_fAlpha = 1.0f;


	Set_Active(false);

	//Appear();

	m_fSizeSpeed = 100.0f;

	return S_OK;
}

void CUI_MatchingLobbyTitle::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_MatchingLobbyTitle::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUI_MatchingLobbyTitle::Render()
{
	__super::Render();

	return S_OK;
}

void CUI_MatchingLobbyTitle::Appear()
{
	m_bActive = true;
	m_eState = UISTATE::APPEAR;
}

void CUI_MatchingLobbyTitle::UI_AppearTick(_float fTimeDelta)
{
	m_eState = UISTATE::TICK;
}

void CUI_MatchingLobbyTitle::UI_Tick(_float fTimeDelta)
{
}

void CUI_MatchingLobbyTitle::UI_DisappearTick(_float fTimeDelta)
{
}



HRESULT CUI_MatchingLobbyTitle::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MatchingLobbyTitle"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_MatchingLobbyTitle::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}


CUI_MatchingLobbyTitle * CUI_MatchingLobbyTitle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_MatchingLobbyTitle*	pInstance = new CUI_MatchingLobbyTitle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_MatchingLobbyTitle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_MatchingLobbyTitle::Clone(void* pArg)
{
	CUI_MatchingLobbyTitle*	pInstance = new CUI_MatchingLobbyTitle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_MatchingLobbyTitle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MatchingLobbyTitle::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
