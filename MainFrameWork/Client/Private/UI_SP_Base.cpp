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


	m_fSizeX = 512 * 0.7f * g_fSizeRatio;
	m_fSizeY = 128 * 0.7f * g_fSizeRatio;
	m_fX = 227.0f * g_fSizeRatio;
	m_fY = 641.0f * g_fSizeRatio;

	m_strObjectTag = L"SP_Base";

	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.001f));

	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);


	m_fAlpha = 0.8f;

	m_bActive = false;


	m_fRankSizeX = 75.6f * g_fSizeRatio;
	m_fRankSizeY = 74.8f * g_fSizeRatio;
	m_fRankX = m_fX - (109.2f * g_fSizeRatio);
	m_fRankY = m_fY + (6.4f * g_fSizeRatio);


	m_pRankTransform->Set_Scale(Vec3(m_fRankSizeX, m_fRankSizeY, 1.f));
	m_pRankTransform->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fRankX - g_iWinSizeX * 0.5f, -m_fRankY + g_iWinSizeY * 0.5f, 0.001f));



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


	_float fAlpha = 1.0f;
	{

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pRankTransform->Get_WorldMatrix())))
			return S_OK;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &fAlpha, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pRankTexture->Set_SRV(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pVIBufferCom->Render();
	}

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

	if (KEY_TAP(KEY::LEFT_ARROW))
	{
		m_fRankX -= 1.0f;
	}

	if (KEY_TAP(KEY::RIGHT_ARROW))
	{
		m_fRankX += 1.0f;
	}

	if (KEY_TAP(KEY::UP_ARROW))
	{
		m_fRankY -= 1.0f;
	}

	if (KEY_TAP(KEY::DOWN_ARROW))
	{
		m_fRankY += 1.0f;
	}

	if (KEY_TAP(KEY::H))
	{
		m_fRankSizeX -= 1.0f;
	}

	if (KEY_TAP(KEY::J))
	{
		m_fRankSizeX += 1.0f;
	}

	if (KEY_TAP(KEY::K))
	{
		m_fRankSizeY -= 1.0f;
	}

	if (KEY_TAP(KEY::L))
	{
		m_fRankSizeY += 1.0f;
	}


	m_pRankTransform->Set_Scale(Vec3(m_fRankSizeX, m_fRankSizeY, 1.f));
	m_pRankTransform->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fRankX - g_iWinSizeX * 0.5f, -m_fRankY + g_iWinSizeY * 0.5f, m_vUITargetPos.z));

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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Rank"),
		TEXT("Com_RankTexture"), (CComponent**)&m_pRankTexture)))
		return E_FAIL;


	/* Com_Transform */
	CTransform::tagTransformDesc		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_LockFree_Transform"),
		TEXT("Com_RankTransform"), (CComponent**)&m_pRankTransform, &TransformDesc)))
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
