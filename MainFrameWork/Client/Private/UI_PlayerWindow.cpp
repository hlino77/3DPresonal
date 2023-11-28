#include "stdafx.h"
#include "UI_PlayerWindow.h"
#include "GameInstance.h"
#include "UI_PlayerWindowTitle.h"

CUI_PlayerWindow::CUI_PlayerWindow(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_PlayerWindow::CUI_PlayerWindow(const CGameObject & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_PlayerWindow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_PlayerWindow::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	__super::Initialize(pArg);


	m_vUITargetPos = Vec3(303.f, 332.f, 1.0f);

	m_vUITargetPos.x *= g_fSizeRatio;
	m_vUITargetPos.y *= g_fSizeRatio;

	m_fSizeX = 381 * g_fSizeRatio;
	m_fSizeY = 338 * g_fSizeRatio;
	m_fX = 100.0f * g_fSizeRatio;
	m_fY = m_vUITargetPos.y;

	m_strObjectTag = L"PlayerWindow";

	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, m_vUITargetPos.z));

	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);


	m_fMoveSpeed = 500.0f;

	m_fAlpha = 0.0f;

	Appear();



	return S_OK;
}

void CUI_PlayerWindow::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_PlayerWindow::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUI_PlayerWindow::Render()
{
	__super::Render();

	return S_OK;
}

void CUI_PlayerWindow::Appear()
{
	m_bActive = true;
	m_eState = UISTATE::APPEAR;
}

void CUI_PlayerWindow::UI_AppearTick(_float fTimeDelta)
{
	m_fAlpha += 0.02f;
	if (m_fAlpha >= 1.0f)
		m_fAlpha = 1.0f;

	m_fX += m_fMoveSpeed * fTimeDelta;
	if (m_fX >= m_vUITargetPos.x)
		m_fX = m_vUITargetPos.x;

		
	if (m_fAlpha == 1.0f && m_fX == m_vUITargetPos.x)
	{
		m_eState = UISTATE::TICK;


		CUI_PlayerWindowTitle* pWindowTitle = dynamic_cast<CUI_PlayerWindowTitle*>(CGameInstance::GetInstance()->
			Find_GameObejct(CGameInstance::GetInstance()->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_UI, L"PlayerWindowTitle"));

		if (pWindowTitle)
			pWindowTitle->Appear();
	}
		
	


	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, m_vUITargetPos.z));
}

void CUI_PlayerWindow::UI_Tick(_float fTimeDelta)
{

}

void CUI_PlayerWindow::UI_DisappearTick(_float fTimeDelta)
{
}



HRESULT CUI_PlayerWindow::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Window"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_PlayerWindow::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}




CUI_PlayerWindow * CUI_PlayerWindow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_PlayerWindow*	pInstance = new CUI_PlayerWindow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : PlayerWindow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_PlayerWindow::Clone(void* pArg)
{
	CUI_PlayerWindow*	pInstance = new CUI_PlayerWindow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : PlayerWindow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PlayerWindow::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
