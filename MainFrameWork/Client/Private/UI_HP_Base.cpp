#include "stdafx.h"
#include "UI_HP_Base.h"
#include "GameInstance.h"

CUI_HP_Base::CUI_HP_Base(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_HP_Base::CUI_HP_Base(const CGameObject & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_HP_Base::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_HP_Base::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	__super::Initialize(pArg);


	m_fSizeX = 512 * 0.7f;
	m_fSizeY = 64 * 0.7f;
	m_fX = 265.0f;
	m_fY = 658.0f;

	m_strObjectTag = L"HP_Base";

	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.0003f));




	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);


	m_fAlpha = 1.0f;

	m_vHp_BackColor = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
	m_vHp_GaugeColor = Vec4(0.0f, 1.0f, 0.0f, 1.0f);

	m_bActive = false;


	return S_OK;
}

void CUI_HP_Base::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_HP_Base::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUI_HP_Base::Render()
{
	__super::Render();

	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_vHp_BackColor, sizeof(Vec4))))
			return E_FAIL;

		m_pHP_Back->Set_SRV(m_pShaderCom, "g_DiffuseTexture");

		m_pShaderCom->Begin(2);

		m_pVIBufferCom->Render();
	}


	{

		if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_vHp_GaugeColor, sizeof(Vec4))))
			return E_FAIL;

		m_pHP_GaugeMask->Set_SRV(m_pShaderCom, "g_MaskTexture");

		m_pHP_Gauge->Set_SRV(m_pShaderCom, "g_DiffuseTexture");

		m_pShaderCom->Begin(3);

		m_pVIBufferCom->Render();
	}




	return S_OK;
}

void CUI_HP_Base::Appear()
{
	m_bActive = true;
	m_eState = UISTATE::APPEAR;
}

void CUI_HP_Base::UI_AppearTick(_float fTimeDelta)
{
	m_eState = UISTATE::TICK;
}

void CUI_HP_Base::UI_Tick(_float fTimeDelta)
{

	/*if (KEY_TAP(KEY::LEFT_ARROW))
	{
		m_fX -= 1.0f;
	}

	if (KEY_TAP(KEY::RIGHT_ARROW))
	{
		m_fX += 1.0f;
	}

	if (KEY_TAP(KEY::UP_ARROW))
	{
		m_fY -= 1.0f;
	}

	if (KEY_TAP(KEY::DOWN_ARROW))
	{
		m_fY += 1.0f;
	}

	if (KEY_TAP(KEY::Y))
	{
		cout << "Pos : " << m_fX << " " << m_fY << endl;
		cout << "Size : " << m_fSizeX << " " << m_fSizeY << endl;
	}


	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, m_vUITargetPos.z));*/
}

void CUI_HP_Base::UI_DisappearTick(_float fTimeDelta)
{
}



HRESULT CUI_HP_Base::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HP_Base"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HP_Back"),
		TEXT("Com_Texture_HP_Back"), (CComponent**)&m_pHP_Back)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HP_Gauge"),
		TEXT("Com_Texture_HP_Gauge"), (CComponent**)&m_pHP_Gauge)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HP_GaugeMask"),
		TEXT("Com_Texture_HP_GaugeMask"), (CComponent**)&m_pHP_GaugeMask)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_HP_Base::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;


	return S_OK;
}




CUI_HP_Base * CUI_HP_Base::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_HP_Base*	pInstance = new CUI_HP_Base(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : HP_Base");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_HP_Base::Clone(void* pArg)
{
	CUI_HP_Base*	pInstance = new CUI_HP_Base(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : HP_Base");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_HP_Base::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
