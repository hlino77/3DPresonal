#include "stdafx.h"
#include "UI_BattleStart.h"
#include "GameInstance.h"

CUI_BattleStart::CUI_BattleStart(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_BattleStart::CUI_BattleStart(const CGameObject & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_BattleStart::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_BattleStart::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	__super::Initialize(pArg);


	m_strObjectTag = L"BattleStart";


	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(0.0f , 0.0f, 0.0f));

	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fMoveSpeed = 500.0f;


	m_fAlpha = 0.0f;


	m_bActive = false;



	return S_OK;
}

void CUI_BattleStart::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_BattleStart::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUI_BattleStart::Render()
{
	__super::Render();

	return S_OK;
}

void CUI_BattleStart::Appear()
{
	m_bActive = true;
	m_eState = UISTATE::APPEAR;


	m_fSizeX = 3000.0f * g_fSizeRatio;
	m_fSizeY = 600.0f * g_fSizeRatio;
	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));

	m_fAlpha = 0.0f;
}

void CUI_BattleStart::UI_AppearTick(_float fTimeDelta)
{
	m_fAlpha += 6.0f * fTimeDelta;

	if (m_fAlpha > 1.0f)
		m_fAlpha = 1.0f;

	m_fSizeX -= 4800.0f * fTimeDelta;

	if (m_fSizeX < 1500.0f)
		m_fSizeX = 1500.0f;

	m_fSizeY -= 960.0f * fTimeDelta;

	if (m_fSizeY < 300.0f)
		m_fSizeY = 300.0f;

	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));

	if (m_fSizeX == 1500.0f && m_fSizeY == 300.0f)
	{
		m_eState = UISTATE::TICK;
	}


}

void CUI_BattleStart::UI_Tick(_float fTimeDelta)
{
	m_fSizeX -= 60.0f * fTimeDelta;

	m_fSizeY -= 12.0f * fTimeDelta;

	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));
}

void CUI_BattleStart::UI_DisappearTick(_float fTimeDelta)
{
	m_fAlpha -= 3.0f * fTimeDelta;

	m_fSizeX -= 2400.0f * fTimeDelta;

	m_fSizeY -= 480.0f * fTimeDelta;


	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));

	if (m_fAlpha < 0.0f)
	{
		m_fAlpha = 0.0f;
		Set_Active(false);
	}
}



HRESULT CUI_BattleStart::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BattleStart"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_BattleStart::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return S_OK;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;



	m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture");

	return S_OK;
}


CUI_BattleStart * CUI_BattleStart::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_BattleStart*	pInstance = new CUI_BattleStart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : TitleCloud");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_BattleStart::Clone(void* pArg)
{
	CUI_BattleStart*	pInstance = new CUI_BattleStart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : TitleCloud");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_BattleStart::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
