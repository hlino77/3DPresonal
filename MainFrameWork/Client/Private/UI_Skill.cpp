#include "stdafx.h"
#include "UI_Skill.h"
#include "GameInstance.h"
#include "Player.h"
#include "ServerSessionManager.h"

CUI_Skill::CUI_Skill(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_Skill::CUI_Skill(const CGameObject & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_Skill::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Skill::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	__super::Initialize(pArg);


	SKILLUIDESC* pDesc = static_cast<SKILLUIDESC*>(pArg);
	
	m_iSkillIndex = pDesc->iSkillIndex;
	m_iTextureIndex = pDesc->iSkillTexture;
	m_strObjectTag = pDesc->szUIName;
	m_pPlayer = pDesc->pPlayer;

	m_fSizeX = 70.0f * g_fSizeRatio;
	m_fSizeY = 70.0f * g_fSizeRatio;
	m_fX = (1100.0f + (m_iSkillIndex * 100.0f)) * g_fSizeRatio;
	m_fY = 640.0f * g_fSizeRatio;


	m_pTransformCom->Set_Scale(Vec3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.0003f));


	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);


	m_fAlpha = 1.0f;


	m_bActive = false;


	m_fMaskUVLength = 0.435f;
	m_vMaskUV = Vec2(0.0f, -m_fMaskUVLength);

	return S_OK;
}

void CUI_Skill::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CUI_Skill::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUI_Skill::Render()
{
	__super::Render();

	{
		if(FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pVIBufferCom->Render();
	}


	{
		if (FAILED(m_pSkill_Mask->Set_SRV(m_pShaderCom, "g_MaskTexture")))
			return E_FAIL;
		if (FAILED(m_pSkill_ShadowMask->Set_SRV(m_pShaderCom, "g_MaskTexture2")))
			return E_FAIL;
		if (FAILED(m_pSkill_Texture->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndex)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaskUV", &m_vMaskUV, sizeof(Vec2))))
			return E_FAIL;


		m_pShaderCom->Begin(4);

		m_pVIBufferCom->Render();
	}

	return S_OK;
}

void CUI_Skill::Appear()
{
	m_bActive = true;
	m_eState = UISTATE::APPEAR;
}

void CUI_Skill::UI_AppearTick(_float fTimeDelta)
{
	m_eState = UISTATE::TICK;
}

void CUI_Skill::UI_Tick(_float fTimeDelta)
{
	if (m_pPlayer->Get_SkillInfo(m_iSkillIndex).m_bReady)
	{
		m_vMaskUV = Vec2(0.0f, m_fMaskUVLength);
	}
	else
	{
		_float fCurrCoolTime = m_pPlayer->Get_SkillInfo(m_iSkillIndex).m_fCurrCoolTime;
		_float fCoolTime = m_pPlayer->Get_SkillInfo(m_iSkillIndex).m_fCoolTime;
		_float fRatio = fCurrCoolTime / fCoolTime;

		_float fCurrUVY = -m_fMaskUVLength + ((m_fMaskUVLength * 2.0f) * fRatio);
		m_vMaskUV = Vec2(0.0f, fCurrUVY);
	}
}

void CUI_Skill::UI_DisappearTick(_float fTimeDelta)
{
}



HRESULT CUI_Skill::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Skill_Base"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Skill_Mask"),
		TEXT("Com_Texture_Skill_Mask"), (CComponent**)&m_pSkill_Mask)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Skill_Icon"),
		TEXT("Com_Texture_Skill_Icon"), (CComponent**)&m_pSkill_Texture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Skill_ShadowMask"),
		TEXT("Com_Texture_Skill_Shadow"), (CComponent**)&m_pSkill_ShadowMask)))
		return E_FAIL;



	return S_OK;
}

HRESULT CUI_Skill::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;


	return S_OK;
}




CUI_Skill * CUI_Skill::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Skill*	pInstance = new CUI_Skill(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Skill");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Skill::Clone(void* pArg)
{
	CUI_Skill*	pInstance = new CUI_Skill(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Skill");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Skill::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
