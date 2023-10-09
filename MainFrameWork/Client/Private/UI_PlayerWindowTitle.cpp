#include "stdafx.h"
#include "UI_PlayerWindowTitle.h"
#include "GameInstance.h"

CUI_PlayerWindowTitle::CUI_PlayerWindowTitle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_PlayerWindowTitle::CUI_PlayerWindowTitle(const CGameObject & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_PlayerWindowTitle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_PlayerWindowTitle::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	__super::Initialize(pArg);

	m_vUITargetPos = Vec3(303.f, 193.f, 0.2f);

	m_fSizeX = 331;
	m_fSizeY = 38;
	m_fX = 250.0f;
	m_fY = m_vUITargetPos.y;

	m_strObjectTag = L"PlayerWindowTitle";

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

void CUI_PlayerWindowTitle::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_PlayerWindowTitle::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUI_PlayerWindowTitle::Render()
{
	__super::Render();

	if (m_eState == UISTATE::TICK)
		Render_String();

	return S_OK;
}

void CUI_PlayerWindowTitle::Appear()
{
	m_bActive = true;
	m_eState = UISTATE::APPEAR;
}

void CUI_PlayerWindowTitle::UI_AppearTick(_float fTimeDelta)
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

void CUI_PlayerWindowTitle::UI_Tick(_float fTimeDelta)
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
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, m_vUITargetPos.z));*/




}

void CUI_PlayerWindowTitle::UI_DisappearTick(_float fTimeDelta)
{
}



HRESULT CUI_PlayerWindowTitle::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_WindowTitle"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_PlayerWindowTitle::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CUI_PlayerWindowTitle::Render_String()
{
	wstring szString = L"플레이어";

	Vec2 vPos(m_fX, m_fY);
	Vec2 vScale(0.5f, 0.5f);

	Vec2 vMeasure = CGameInstance::GetInstance()->MeasureString(L"125", szString);
	Vec2 vOrigin = vMeasure * 0.5f;

	CGameInstance::GetInstance()->DrawFont(L"125", szString, vPos, Vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, vOrigin, vScale);
}




CUI_PlayerWindowTitle * CUI_PlayerWindowTitle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_PlayerWindowTitle*	pInstance = new CUI_PlayerWindowTitle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : PlayerWindowTitle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_PlayerWindowTitle::Clone(void* pArg)
{
	CUI_PlayerWindowTitle*	pInstance = new CUI_PlayerWindowTitle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : PlayerWindowTitle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_PlayerWindowTitle::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
