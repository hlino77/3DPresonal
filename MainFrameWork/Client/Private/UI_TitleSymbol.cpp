#include "stdafx.h"
#include "UI_TitleSymbol.h"
#include "GameInstance.h"

CUI_TitleSymbol::CUI_TitleSymbol(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_TitleSymbol::CUI_TitleSymbol(const CGameObject & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_TitleSymbol::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_TitleSymbol::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	__super::Initialize(pArg);

	m_vUITargetSize = Vec2(75.f, 75.f);

	m_vUITargetSize *= g_fSizeRatio;

	m_fSizeX = 100 * g_fSizeRatio;
	m_fSizeY = 100 * g_fSizeRatio;
	m_fX = 45.0f * g_fSizeRatio;
	m_fY = 53.0f * g_fSizeRatio;

	m_strObjectTag = L"TitleSymbol";

	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.01f));

	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fAlpha = 0.0f;


	Set_Active(false);

	//Appear();

	m_fSizeSpeed = 100.0f;

	return S_OK;
}

void CUI_TitleSymbol::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_TitleSymbol::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUI_TitleSymbol::Render()
{
	__super::Render();

	return S_OK;
}

void CUI_TitleSymbol::Appear()
{
	m_bActive = true;
	m_eState = UISTATE::APPEAR;
}

void CUI_TitleSymbol::UI_AppearTick(_float fTimeDelta)
{
	m_fSizeX -= m_fSizeSpeed * fTimeDelta;
	m_fSizeY -= m_fSizeSpeed * fTimeDelta;

	if (m_fSizeX <= m_vUITargetSize.x)
		m_fSizeX = m_vUITargetSize.x;

	if (m_fSizeY <= m_vUITargetSize.y)
		m_fSizeY = m_vUITargetSize.y;


	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));

	m_fAlpha += 0.03f;
	if (m_fAlpha >= 1.0f)
		m_fAlpha = 1.0f;
	
	

	if (m_fSizeY == m_vUITargetSize.y &&
		m_fSizeX == m_vUITargetSize.x &&
		m_fAlpha == 1.0f)
	{
		m_eState = UISTATE::TICK;
	}

}

void CUI_TitleSymbol::UI_Tick(_float fTimeDelta)
{
}

void CUI_TitleSymbol::UI_DisappearTick(_float fTimeDelta)
{
}



HRESULT CUI_TitleSymbol::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TitleSymbol"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_TitleSymbol::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}


CUI_TitleSymbol * CUI_TitleSymbol::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_TitleSymbol*	pInstance = new CUI_TitleSymbol(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : TitleCloud");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_TitleSymbol::Clone(void* pArg)
{
	CUI_TitleSymbol*	pInstance = new CUI_TitleSymbol(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : TitleCloud");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_TitleSymbol::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
