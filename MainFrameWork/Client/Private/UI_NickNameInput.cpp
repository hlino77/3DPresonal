#include "stdafx.h"
#include "UI_NickNameInput.h"

#include "GameInstance.h"
#include "ServerSessionManager.h"

CUI_NickNameInput::CUI_NickNameInput(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_NickNameInput::CUI_NickNameInput(const CGameObject & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_NickNameInput::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_NickNameInput::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	__super::Initialize(pArg);

	m_fSizeX = 250.0f * g_fSizeRatio;
	m_fSizeY = 30.0f * g_fSizeRatio;
	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.5f + (10.0f * g_fSizeRatio);

	m_strObjectTag = L"NickNameInput";

	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.8f));

	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);



	m_bActive = false;

	return S_OK;
}

void CUI_NickNameInput::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	
}

void CUI_NickNameInput::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUI_NickNameInput::Render()
{
	__super::Render();

	Render_String();

	return S_OK;
}


void CUI_NickNameInput::UI_Tick(_float fTimeDelta)
{
	if(m_bNickName == false)
	{
		wstring szPrev = m_szNickName;

		CGameInstance::GetInstance()->InputText(m_szNickName);

		if (szPrev != m_szNickName)
			CGameInstance::GetInstance()->PlaySoundFile(L"UI_Click.wav", CHANNELID::CHANNEL_UI, g_fVolume * 0.2f);

		if (KEY_TAP(KEY::ENTER))
		{
			if (!m_szNickName.empty())
			{
				CServerSessionManager::GetInstance()->Set_NickName(m_szNickName);
				CGameInstance::GetInstance()->PlaySoundFile(L"UI_Select.wav", CHANNELID::CHANNEL_UI, g_fVolume * 0.2f);
				m_bNickName = true;
			}
		}
	}
}

HRESULT CUI_NickNameInput::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_NickNameInput"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_NickNameInput::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CUI_NickNameInput::Render_String()
{
	Vec2 vPos(m_fX, m_fY);
	Vec2 vScale(0.7f, 0.7f);

	Vec2 vMeasure = CGameInstance::GetInstance()->MeasureString(L"Cascadia", m_szNickName);
	Vec2 vOrigin = vMeasure * 0.5f;

	CGameInstance::GetInstance()->DrawFont(L"Cascadia", m_szNickName, vPos, Vec4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, vOrigin, vScale);
}

CUI_NickNameInput * CUI_NickNameInput::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NickNameInput*	pInstance = new CUI_NickNameInput(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_NickName");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_NickNameInput::Clone(void* pArg)
{
	CUI_NickNameInput*	pInstance = new CUI_NickNameInput(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_NickName");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_NickNameInput::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
