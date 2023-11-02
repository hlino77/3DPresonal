#include "stdafx.h"
#include "UI_SP_Base.h"
#include "GameInstance.h"

CUI_SP_Base::CUI_SP_Base(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_SP_Base::CUI_SP_Base(const CGameObject & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_SP_Base::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_SP_Base::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	__super::Initialize(pArg);


	m_fSizeX = 512 * 0.7f;
	m_fSizeY = 128 * 0.7f;
	m_fX = 227.0f;
	m_fY = 641.0f;

	m_strObjectTag = L"SP_Base";

	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.001f));

	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);


	m_fAlpha = 0.8f;

	m_bActive = false;


	return S_OK;
}

void CUI_SP_Base::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_SP_Base::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUI_SP_Base::Render()
{
	__super::Render();

	return S_OK;
}

void CUI_SP_Base::Appear()
{
	m_bActive = true;
	m_eState = UISTATE::APPEAR;
}

void CUI_SP_Base::UI_AppearTick(_float fTimeDelta)
{
	m_eState = UISTATE::TICK;
}

void CUI_SP_Base::UI_Tick(_float fTimeDelta)
{
	
}

void CUI_SP_Base::UI_DisappearTick(_float fTimeDelta)
{
}



HRESULT CUI_SP_Base::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_SP_Base"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_SP_Base::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}




CUI_SP_Base * CUI_SP_Base::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_SP_Base*	pInstance = new CUI_SP_Base(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : SP_Base");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_SP_Base::Clone(void* pArg)
{
	CUI_SP_Base*	pInstance = new CUI_SP_Base(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : SP_Base");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_SP_Base::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
