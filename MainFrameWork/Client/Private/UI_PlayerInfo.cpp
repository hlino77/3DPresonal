#include "stdafx.h"
#include "UI_PlayerInfo.h"
#include "GameInstance.h"
#include "ServerSessionManager.h"
#include "UI_CharacterSelect.h"
#include "Level_Lobby.h"
#include "LobbyUser.h"

CUI_PlayerInfo::CUI_PlayerInfo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_PlayerInfo::CUI_PlayerInfo(const CGameObject & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_PlayerInfo::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_PlayerInfo::Initialize(void* pArg)
{
	PlayerInfoUI* tPlayerInfo = static_cast<PlayerInfoUI*>(pArg);

	m_szNickName = tPlayerInfo->szNickName;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	__super::Initialize(pArg);

	_uint iIndex = tPlayerInfo->iIndex;

	m_vUITargetPos = Vec3(303.f, 259.f + iIndex * 80.0f, 0.05f);

	m_vUITargetPos.x *= g_fSizeRatio;
	m_vUITargetPos.y *= g_fSizeRatio;


	m_fSizeX = 286.f * g_fSizeRatio;
	m_fSizeY = 64.f * g_fSizeRatio;
	m_fX = 250.0f * g_fSizeRatio;
	m_fY = m_vUITargetPos.y;

	m_strObjectTag = L"PlayerInfo_" + m_szNickName;

	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, m_vUITargetPos.z));

	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);


	//CharacterTexture
	m_fTextureSizeX = 93.f * g_fSizeRatio;
	m_fTextureSizeY = 85.f * g_fSizeRatio;
	m_fTextureX = m_fX - (109.f * g_fSizeRatio);
	m_fTextureY = m_fY;


	m_pTextureTransform->Set_Scale(Vec3(m_fTextureSizeX, m_fTextureSizeY, 1.f));
	m_pTextureTransform->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fTextureX - g_iWinSizeX * 0.5f, -m_fTextureY + g_iWinSizeY * 0.5f, m_vUITargetPos.z - 0.01f));


	//ReadyMark
	m_fMarkSizeX = 20.f * g_fSizeRatio;
	m_fMarkSizeY = 20.f * g_fSizeRatio;
	m_fMarkX = m_vUITargetPos.x + (125.f * g_fSizeRatio);
	m_fMarkY = m_vUITargetPos.y + (2.0f * g_fSizeRatio);


	m_pMarkTransform->Set_Scale(Vec3(m_fMarkSizeX, m_fMarkSizeY, 1.f));
	m_pMarkTransform->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fMarkX - g_iWinSizeX * 0.5f, -m_fMarkY + g_iWinSizeY * 0.5f, m_vUITargetPos.z - 0.01f));


	m_fMoveSpeed = 200.0f;

	m_fAlpha = 0.0f;

	Set_Active(false);

	return S_OK;
}

void CUI_PlayerInfo::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_PlayerInfo::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUI_PlayerInfo::Render()
{
	__super::Render();

	if (m_eState == UISTATE::TICK)
		Render_String();
		
	Render_CharacterTexture();


	if (m_bReady)
		Render_ReadyMark();

	return S_OK;
}

void CUI_PlayerInfo::Appear()
{
	m_bActive = true;
	m_eState = UISTATE::APPEAR;

	CGameInstance::GetInstance()->PlaySoundFile(L"MatchingLobbyStart.wav", CHANNELID::CHANNEL_UI, g_fVolume * 0.2f);
}

void CUI_PlayerInfo::Set_TextureIndex(const wstring& szCharacterName)
{
	if (szCharacterName == L"Naruto")
		m_iTextureIndex = 1;
	else if (szCharacterName == L"Sasuke")
		m_iTextureIndex = 3;
	else
		m_iTextureIndex = 0;
}

void CUI_PlayerInfo::UI_AppearTick(_float fTimeDelta)
{
	m_fAlpha += 0.03f;
	if (m_fAlpha >= 1.0f)
		m_fAlpha = 1.0f;

	m_fX += m_fMoveSpeed * fTimeDelta;
	if (m_fX >= m_vUITargetPos.x)
		m_fX = m_vUITargetPos.x;	


	if (m_fAlpha == 1.0f && m_fX == m_vUITargetPos.x)
	{
		m_eState = UISTATE::TICK;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, m_vUITargetPos.z));

	m_fTextureX = m_fX - (109.f * g_fSizeRatio);
	m_fTextureY = m_fY;


	m_pTextureTransform->Set_Scale(Vec3(m_fTextureSizeX, m_fTextureSizeY, 1.f));
	m_pTextureTransform->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fTextureX - g_iWinSizeX * 0.5f, -m_fTextureY + g_iWinSizeY * 0.5f, m_vUITargetPos.z - 0.01f));

}

void CUI_PlayerInfo::UI_Tick(_float fTimeDelta)
{
	CLevel_Lobby* pLevelLobby = dynamic_cast<CLevel_Lobby*>(CGameInstance::GetInstance()->Get_CurrLevel());

	if (pLevelLobby)
	{
		if (pLevelLobby->Find_LobbyUser(m_szNickName)->Is_Ready())
		{
			if (m_bReady == false)
			{
				m_bReady = true;
				CGameInstance::GetInstance()->PlaySoundFile(L"UI_Ready.wav", CHANNELID::CHANNEL_UI, g_fVolume * 0.2f);
			}
		}
	}
		
}

void CUI_PlayerInfo::UI_DisappearTick(_float fTimeDelta)
{
}



HRESULT CUI_PlayerInfo::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture*/
	if (m_szNickName == CServerSessionManager::GetInstance()->Get_NickName())
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerInfo"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_OtherPlayerInfo"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Character"),
		TEXT("Com_CharacterTexture"), (CComponent**)&m_pCharacterTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ReadyMark"),
		TEXT("Com_MarkTexture"), (CComponent**)&m_pMarkTexture)))
		return E_FAIL;

	{
		CTransform::tagTransformDesc		TransformDesc;
		ZeroMemory(&TransformDesc, sizeof TransformDesc);

		TransformDesc.fSpeedPerSec = 5.f;
		TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_UseLock_Transform"),
			TEXT("Com_CharacterTransform"), (CComponent**)&m_pTextureTransform, &TransformDesc)))
			return E_FAIL;
	}
	

	{
		CTransform::tagTransformDesc		TransformDesc;
		ZeroMemory(&TransformDesc, sizeof TransformDesc);

		TransformDesc.fSpeedPerSec = 5.f;
		TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_UseLock_Transform"),
			TEXT("Com_MarkTransform"), (CComponent**)&m_pMarkTransform, &TransformDesc)))
			return E_FAIL;
	}



	
	return S_OK;
}

HRESULT CUI_PlayerInfo::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CUI_PlayerInfo::Render_String()
{
	Vec2 vPos(m_fX, m_fY);
	Vec2 vScale(0.65f, 0.65f);

	Vec2 vMeasure = CGameInstance::GetInstance()->MeasureString(L"Cascadia", m_szNickName);
	Vec2 vOrigin = vMeasure * 0.5f;

	CGameInstance::GetInstance()->DrawFont(L"Cascadia", m_szNickName, vPos, Vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, vOrigin, vScale);
}

HRESULT CUI_PlayerInfo::Render_CharacterTexture()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTextureTransform->Get_WorldMatrix())))
		return S_OK;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;


	m_pCharacterTexture->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndex);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();
}

HRESULT CUI_PlayerInfo::Render_ReadyMark()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pMarkTransform->Get_WorldMatrix())))
		return S_OK;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;


	m_pMarkTexture->Set_SRV(m_pShaderCom, "g_DiffuseTexture");

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();


	return S_OK;
}



CUI_PlayerInfo * CUI_PlayerInfo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_PlayerInfo*	pInstance = new CUI_PlayerInfo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : PlayerInfo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_PlayerInfo::Clone(void* pArg)
{
	CUI_PlayerInfo*	pInstance = new CUI_PlayerInfo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : PlayerInfo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PlayerInfo::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
