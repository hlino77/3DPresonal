#include "stdafx.h"
#include "UI_TitleCloud.h"
#include "GameInstance.h"

CUI_TitleCloud::CUI_TitleCloud(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_TitleCloud::CUI_TitleCloud(const CGameObject & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_TitleCloud::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_TitleCloud::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	__super::Initialize(pArg);


	m_tCloudInfo = *(static_cast<TitleCloud*>(pArg));

	m_fSizeX = m_tCloudInfo.vSize.x;
	m_fSizeY = m_tCloudInfo.vSize.y;
	m_fX = m_tCloudInfo.vTargetPos.x * 0.3f;
	m_fY = m_tCloudInfo.vTargetPos.y;

	m_strObjectTag = L"TitleCloud";

	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, m_tCloudInfo.vTargetPos.z));

	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);

	m_fMoveSpeed = 500.0f;


	m_fAlpha = 0.0f;

	Appear();



	return S_OK;
}

void CUI_TitleCloud::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_TitleCloud::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUI_TitleCloud::Render()
{
	__super::Render();

	return S_OK;
}

void CUI_TitleCloud::Appear()
{
	m_bActive = true;
	m_eState = UISTATE::APPEAR;
}

void CUI_TitleCloud::UI_AppearTick(_float fTimeDelta)
{
	m_fAlpha = m_fX / m_tCloudInfo.vTargetPos.x;

	m_fX += m_fMoveSpeed * fTimeDelta;

	if (m_fX >= m_tCloudInfo.vTargetPos.x)
	{
		m_fX = m_tCloudInfo.vTargetPos.x;
		m_eState = UISTATE::TICK;
	}


	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, m_tCloudInfo.vTargetPos.z));
}

void CUI_TitleCloud::UI_Tick(_float fTimeDelta)
{
}

void CUI_TitleCloud::UI_DisappearTick(_float fTimeDelta)
{
}



HRESULT CUI_TitleCloud::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TitleCloud"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_TitleCloud::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return S_OK;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;



	m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_tCloudInfo.iTexture);

	return S_OK;
}


CUI_TitleCloud * CUI_TitleCloud::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_TitleCloud*	pInstance = new CUI_TitleCloud(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : TitleCloud");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_TitleCloud::Clone(void* pArg)
{
	CUI_TitleCloud*	pInstance = new CUI_TitleCloud(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : TitleCloud");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_TitleCloud::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
