#include "stdafx.h"
#include "UI_Title.h"
#include "GameInstance.h"
#include "UI_TitleSymbol.h"

CUI_Title::CUI_Title(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_Title::CUI_Title(const CGameObject & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_Title::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Title::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	__super::Initialize(pArg);

	m_vUITargetPos = Vec3(225.0f, 53.0f, 0.02f);

	m_fSizeX = 475;
	m_fSizeY = 60;
	m_fX = 100.0f;
	m_fY = m_vUITargetPos.y;

	m_strObjectTag = L"Title";

	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, m_vUITargetPos.z));

	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);


	m_fMoveSpeed = 300.0f;

	m_fAlpha = 0.0f;

	Appear();



	return S_OK;
}

void CUI_Title::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Title::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUI_Title::Render()
{
	__super::Render();


	if (m_eState == UISTATE::TICK)
		Render_String();

	return S_OK;
}

void CUI_Title::Appear()
{
	m_bActive = true;
	m_eState = UISTATE::APPEAR;
}

void CUI_Title::UI_AppearTick(_float fTimeDelta)
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

		CUI_TitleSymbol* pTitleSymbol = dynamic_cast<CUI_TitleSymbol*>(CGameInstance::GetInstance()->
			Find_GameObejct(CGameInstance::GetInstance()->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_UI, L"TitleSymbol"));

		if (pTitleSymbol)
			pTitleSymbol->Appear();
	}
		
	


	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, m_vUITargetPos.z));
}

void CUI_Title::UI_Tick(_float fTimeDelta)
{


}

void CUI_Title::UI_DisappearTick(_float fTimeDelta)
{
}



HRESULT CUI_Title::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Title"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Title::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CUI_Title::Render_String()
{
	wstring szString = L"캐릭터 선택";

	Vec2 vPos(m_fX - 150.0f, m_fY);
	Vec2 vScale(0.7f, 0.7f);

	Vec2 vMeasure = CGameInstance::GetInstance()->MeasureString(L"125", szString);
	Vec2 vOrigin(0.0f, vMeasure.y * 0.5f);

	CGameInstance::GetInstance()->DrawFont(L"125", szString, vPos, Vec4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, vOrigin, vScale);
}


CUI_Title * CUI_Title::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Title*	pInstance = new CUI_Title(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : TitleCloud");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Title::Clone(void* pArg)
{
	CUI_Title*	pInstance = new CUI_Title(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : TitleCloud");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Title::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
