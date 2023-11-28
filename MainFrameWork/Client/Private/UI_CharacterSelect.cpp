#include "stdafx.h"
#include "UI_CharacterSelect.h"
#include "GameInstance.h"
#include "LobbyUser.h"
#include "Level_Lobby.h"
#include "ServerSessionManager.h"

CUI_CharacterSelect::CUI_CharacterSelect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_CharacterSelect::CUI_CharacterSelect(const CGameObject & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_CharacterSelect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_CharacterSelect::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	__super::Initialize(pArg);

	CharacterUI* tUI = static_cast<CharacterUI*>(pArg);

	m_tCharacterUI.szCharacterName = tUI->szCharacterName;
	m_tCharacterUI.bSelected = tUI->bSelected;
	m_tCharacterUI.iIndex = tUI->iIndex;
	m_tCharacterUI.iTexture = tUI->iTexture;


	m_vUITargetPos = Vec3(155.f + 100.f * m_tCharacterUI.iIndex, 600.0f, 1.0f);


	m_fSizeX = 100 * g_fSizeRatio;
	m_fSizeY = 100 * g_fSizeRatio;
	m_fX = m_vUITargetPos.x * g_fSizeRatio;
	m_fY = m_vUITargetPos.y * g_fSizeRatio;

	m_strObjectTag = L"CharacterSelect_" + m_tCharacterUI.szCharacterName;

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

void CUI_CharacterSelect::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_CharacterSelect::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUI_CharacterSelect::Render()
{
	__super::Render();

	return S_OK;
}

void CUI_CharacterSelect::Appear()
{
	m_bActive = true;
	m_eState = UISTATE::APPEAR;
}

void CUI_CharacterSelect::Set_Selected(_bool bSelect)
{
	m_tCharacterUI.bSelected = bSelect;

	if (bSelect == true)
	{
		CLevel_Lobby* pLevelLobby = dynamic_cast<CLevel_Lobby*>(CGameInstance::GetInstance()->Get_CurrLevel());

		if (pLevelLobby)
			pLevelLobby->Find_LobbyUser(CServerSessionManager::GetInstance()->Get_NickName())->Set_Character(m_tCharacterUI.szCharacterName);
	}
}

void CUI_CharacterSelect::UI_AppearTick(_float fTimeDelta)
{
	m_fAlpha += 0.02f;
	if (m_fAlpha >= 1.0f)
		m_fAlpha = 1.0f;

		
	if (m_fAlpha == 1.0f)
	{
		m_eState = UISTATE::TICK;


		if (m_tCharacterUI.bSelected)
		{
			CLevel_Lobby* pLevelLobby = dynamic_cast<CLevel_Lobby*>(CGameInstance::GetInstance()->Get_CurrLevel());

			if (pLevelLobby)
				pLevelLobby->Find_LobbyUser(CServerSessionManager::GetInstance()->Get_NickName())->Set_Character(m_tCharacterUI.szCharacterName);
		}
	}
	
}

void CUI_CharacterSelect::UI_Tick(_float fTimeDelta)
{

}

void CUI_CharacterSelect::UI_DisappearTick(_float fTimeDelta)
{
}



HRESULT CUI_CharacterSelect::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Character"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_CharacterSelect::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return S_OK;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;


	if(m_tCharacterUI.bSelected == true)
		m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_tCharacterUI.iTexture + 1);
	else
		m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_tCharacterUI.iTexture);

	return S_OK;
}




CUI_CharacterSelect * CUI_CharacterSelect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_CharacterSelect*	pInstance = new CUI_CharacterSelect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CharacterSelect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_CharacterSelect::Clone(void* pArg)
{
	CUI_CharacterSelect*	pInstance = new CUI_CharacterSelect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CharacterSelect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_CharacterSelect::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
