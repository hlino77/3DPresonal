#include "stdafx.h"
#include "UI_PlayerInfo.h"
#include "GameInstance.h"
#include "ServerSessionManager.h"


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

	m_fSizeX = 286.f;
	m_fSizeY = 64.f;
	m_fX = 250.0f;
	m_fY = m_vUITargetPos.y;

	m_strObjectTag = L"PlayerInfo_" + m_szNickName;

	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, m_vUITargetPos.z));

	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);


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

	return S_OK;
}

void CUI_PlayerInfo::Appear()
{
	m_bActive = true;
	m_eState = UISTATE::APPEAR;
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
}

void CUI_PlayerInfo::UI_Tick(_float fTimeDelta)
{

	if (KEY_TAP(KEY::LEFT_ARROW))
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

	if (KEY_TAP(KEY::U))
	{
		m_fSizeX -= 1.0f;
	}

	if (KEY_TAP(KEY::I))
	{
		m_fSizeX += 1.0f;
	}

	if (KEY_TAP(KEY::O))
	{
		m_fSizeY -= 1.0f;
	}

	if (KEY_TAP(KEY::P))
	{
		m_fSizeY += 1.0f;
	}


	if (KEY_TAP(KEY::Y))
	{
		cout << "Pos : " << m_fX << " " << m_fY << endl;
		cout << "Size : " << m_fSizeX << " " << m_fSizeY << endl;
 	}


	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, m_vUITargetPos.z));




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
	Vec2 vScale(0.5f, 0.5f);

	Vec2 vMeasure = CGameInstance::GetInstance()->MeasureString(L"125", m_szNickName);
	Vec2 vOrigin = vMeasure * 0.5f;

	CGameInstance::GetInstance()->DrawFont(L"125", m_szNickName, vPos, Vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, vOrigin, vScale);
}




CUI_PlayerInfo * CUI_PlayerInfo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_PlayerInfo*	pInstance = new CUI_PlayerInfo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : PlayerWindowTitle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_PlayerInfo::Clone(void* pArg)
{
	CUI_PlayerInfo*	pInstance = new CUI_PlayerInfo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : PlayerWindowTitle");
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
