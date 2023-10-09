#include "stdafx.h"
#include "UI_NickName.h"
#include "UI_NickNameInput.h"
#include "GameInstance.h"

CUI_NickName::CUI_NickName(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_NickName::CUI_NickName(const CGameObject & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_NickName::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_NickName::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	__super::Initialize(pArg);

	m_fSizeX = 300.0f;
	m_fSizeY = 150.0f;
	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.5f;

	m_strObjectTag = L"NickNameDialog";

	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.9f));

	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);



	m_bActive = false;

	return S_OK;
}

void CUI_NickName::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_NickName::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUI_NickName::Render()
{
	__super::Render();

	Render_String();

	return S_OK;
}

void CUI_NickName::Appear()
{
	m_bActive = true;
	m_eState = UISTATE::APPEAR;

	CUI_NickNameInput* pNickNameInput = dynamic_cast<CUI_NickNameInput*>(CGameInstance::GetInstance()->
		Find_GameObejct(CGameInstance::GetInstance()->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_UI, L"NickNameInput"));

	//pNickNameInput->Set_Active(true);
	pNickNameInput->Appear();
}

void CUI_NickName::UI_AppearTick(_float fTimeDelta)
{
}

void CUI_NickName::UI_Tick(_float fTimeDelta)
{
}

void CUI_NickName::UI_DisappearTick(_float fTimeDelta)
{
}



HRESULT CUI_NickName::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_NickNameDialog"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_NickName::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CUI_NickName::Render_String()
{
	wstring szString = L"닉네임을 입력하십시오";

	Vec2 vPos(m_fX, m_fY - 30.0f);
	Vec2 vScale(0.4f, 0.4f);

	Vec2 vMeasure = CGameInstance::GetInstance()->MeasureString(L"125", szString);
	Vec2 vOrigin = vMeasure * 0.5f;

	CGameInstance::GetInstance()->DrawFont(L"125", szString, vPos, Vec4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, vOrigin, vScale);
}

CUI_NickName * CUI_NickName::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NickName*	pInstance = new CUI_NickName(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_NickName");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_NickName::Clone(void* pArg)
{
	CUI_NickName*	pInstance = new CUI_NickName(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_NickName");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_NickName::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
