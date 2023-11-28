#include "stdafx.h"
#include "UI_HP_Monster.h"
#include "GameInstance.h"
#include "ServerSessionManager.h"

CUI_HP_Monster::CUI_HP_Monster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_HP_Monster::CUI_HP_Monster(const CGameObject & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_HP_Monster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_HP_Monster::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	__super::Initialize(pArg);


	m_pMonster = static_cast<CGameObject*>(pArg);
	m_strObjectTag = L"HP_Monster";



	m_fSizeX = 512 * 0.002f * g_fSizeRatio;
	m_fSizeY = 64 * 0.002f * g_fSizeRatio;

	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));

	m_fAlpha = 0.5f;

	m_vHp_GaugeColor = Vec4(1.0f, 0.0f, 0.0f, 1.0f);

	m_bActive = false;


	m_iPrevHp = m_pMonster->Get_Hp();
	
	m_fMaskUVLength = 0.31f;
	m_vMaskUV = Vec2(-m_fMaskUVLength, 0.0f);

	m_vHp_DamageColor = Vec4(1.0f, 0.0f, 0.0f, 1.0f);

	return S_OK;
}

void CUI_HP_Monster::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CUI_HP_Monster::LateTick(_float fTimeDelta)
{
	if (m_bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_WORLDUI, this);
}

HRESULT CUI_HP_Monster::Render()
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

void CUI_HP_Monster::Appear()
{
	m_bActive = true;
	m_eState = UISTATE::APPEAR;
}

void CUI_HP_Monster::UI_AppearTick(_float fTimeDelta)
{
	m_eState = UISTATE::TICK;
}

void CUI_HP_Monster::UI_Tick(_float fTimeDelta)
{
	if (m_pMonster->Is_Active() == false)
		Set_Active(false);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	Vec3 vPos = m_pMonster->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	vPos.y += 1.7f;



	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	Vec3 vCamPos = pGameInstance->Get_CamPosition();
	Vec3 vDir = vCamPos - vPos;
	_float fLength = vDir.Length();

	if (fLength > 25.0f)
		m_bRender = false;
	else
		m_bRender = true;


	m_pTransformCom->LookAt(vPos - vDir);


	Safe_Release(pGameInstance);


	_uint iCurrHp = m_pMonster->Get_Hp();
	_uint iMaxHp = m_pMonster->Get_MaxHp();

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

void CUI_HP_Monster::UI_DisappearTick(_float fTimeDelta)
{
}



HRESULT CUI_HP_Monster::Ready_Components()
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

HRESULT CUI_HP_Monster::Bind_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return S_OK;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;



	Safe_Release(pGameInstance);

	return S_OK;
}




CUI_HP_Monster * CUI_HP_Monster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_HP_Monster*	pInstance = new CUI_HP_Monster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_HP_Monster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_HP_Monster::Clone(void* pArg)
{
	CUI_HP_Monster*	pInstance = new CUI_HP_Monster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_HP_Monster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_HP_Monster::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
