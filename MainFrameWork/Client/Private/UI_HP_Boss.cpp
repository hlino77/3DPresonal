#include "stdafx.h"
#include "UI_HP_Boss.h"
#include "GameInstance.h"
#include "ServerSessionManager.h"

CUI_HP_Boss::CUI_HP_Boss(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_HP_Boss::CUI_HP_Boss(const CGameObject & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_HP_Boss::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_HP_Boss::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	__super::Initialize(pArg);


	m_pBoss = static_cast<CGameObject*>(pArg);
	m_strObjectTag = L"HP_Boss";



	m_fSizeX = 800.f * 1.0f * g_fSizeRatio;
	m_fSizeY = 64.f * 0.7f * g_fSizeRatio;
	m_fX = g_iWinSizeX * 0.5f;
	m_fY = 36.0f * g_fSizeRatio;


	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.0003f));


	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fAlpha = 0.8f;

	m_vHp_GaugeColor = Vec4(1.0f, 0.0f, 0.0f, 1.0f);

	m_bActive = false;


	m_iPrevHp = m_pBoss->Get_Hp();
	
	m_fMaskUVLength = 0.31f;
	m_vMaskUV = Vec2(-m_fMaskUVLength, 0.0f);

	m_vHp_DamageColor = Vec4(1.0f, 0.0f, 0.0f, 1.0f);

	return S_OK;
}

void CUI_HP_Boss::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CUI_HP_Boss::LateTick(_float fTimeDelta)
{
	if (m_bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_WORLDUI, this);
}

HRESULT CUI_HP_Boss::Render()
{
	__super::Render();

	_float fAlpha = 1.0f;
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &fAlpha, sizeof(_float))))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_vHp_DamageColor, sizeof(Vec4))))
			return E_FAIL;

		m_pHP_Damage->Set_SRV(m_pShaderCom, "g_DiffuseTexture");

		m_pShaderCom->Begin(2);

		m_pVIBufferCom->Render();
	}


	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &fAlpha, sizeof(_float))))
			return E_FAIL;



		if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_vHp_GaugeColor, sizeof(Vec4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaskUV", &m_vMaskUV, sizeof(Vec2))))
			return E_FAIL;

		m_pHP_GaugeMask->Set_SRV(m_pShaderCom, "g_MaskTexture");

		m_pHP_Gauge->Set_SRV(m_pShaderCom, "g_DiffuseTexture");

		m_pShaderCom->Begin(3);

		m_pVIBufferCom->Render();
	}




	return S_OK;
}

void CUI_HP_Boss::Appear()
{
	m_bActive = true;
	m_eState = UISTATE::APPEAR;
}

void CUI_HP_Boss::UI_AppearTick(_float fTimeDelta)
{
	m_eState = UISTATE::TICK;
}

void CUI_HP_Boss::UI_Tick(_float fTimeDelta)
{
	if (m_pBoss->Is_Active() == false)
		Set_Active(false);


	_uint iCurrHp = m_pBoss->Get_Hp();
	_uint iMaxHp = m_pBoss->Get_MaxHp();

	_float fRatio = (_float)iCurrHp / iMaxHp;

	_float fCurrUVX = m_fMaskUVLength - ((m_fMaskUVLength * 2.0f) * fRatio);
	
	m_vMaskUV = Vec2::Lerp(m_vMaskUV, Vec2(fCurrUVX, 0.0f), 2.0f * fTimeDelta);


	if (m_vHp_DamageColor.w > 0.0f)
	{
		m_vHp_DamageColor.w -= 1.0f * fTimeDelta;
		if (m_vHp_DamageColor.w < 0.0f)
			m_vHp_DamageColor.w = 0.0f;
	}


	if (iCurrHp != m_iPrevHp)
	{
		m_vHp_DamageColor.w = 1.0f;
		m_iPrevHp = iCurrHp;
	}


}

void CUI_HP_Boss::UI_DisappearTick(_float fTimeDelta)
{
}



HRESULT CUI_HP_Boss::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HP_Base"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HP_Gauge"),
		TEXT("Com_Texture_HP_Gauge"), (CComponent**)&m_pHP_Gauge)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HP_GaugeMask"),
		TEXT("Com_Texture_HP_GaugeMask"), (CComponent**)&m_pHP_GaugeMask)))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HP_Damage"),
		TEXT("Com_Texture_HP_Damage"), (CComponent**)&m_pHP_Damage)))
		return E_FAIL;





	return S_OK;
}

HRESULT CUI_HP_Boss::Bind_ShaderResources()
{
	if(FAILED(__super::Bind_ShaderResources()))

	return S_OK;
}




CUI_HP_Boss * CUI_HP_Boss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_HP_Boss*	pInstance = new CUI_HP_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_HP_Monster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_HP_Boss::Clone(void* pArg)
{
	CUI_HP_Boss*	pInstance = new CUI_HP_Boss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_HP_Monster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_HP_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
