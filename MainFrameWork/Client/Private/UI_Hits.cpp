#include "stdafx.h"
#include "UI_Hits.h"
#include "GameInstance.h"

CUI_Hits::CUI_Hits(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CUI_Hits::CUI_Hits(const CGameObject & rhs)
	: CUI(rhs)
{

}

HRESULT CUI_Hits::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Hits::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	__super::Initialize(pArg);


	m_strObjectTag = L"Hits";


	

	m_pTransformCom->Set_WorldMatrix(Matrix::CreateFromAxisAngle(Vec3(0.0f, 0.0f, -1.0f), -XMConvertToRadians(25.0f)));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		Vec3(250.0f , 100.0f, 0.0f));



	m_vOffsetDir = Vec3(1.0f, 0.0f, 0.0f);


	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);


	matCount = Matrix::CreateScale(108.0f, 128.0f, 1.0f);
	matHit = Matrix::CreateScale(552.0f, 164.0f, 1.0f);
	matHits = Matrix::CreateScale(256.0f, 72.0f, 1.0f);

	m_fAlpha = 0.0f;


	m_bActive = false;



	return S_OK;
}

void CUI_Hits::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_Hits::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);
}

HRESULT CUI_Hits::Render()
{
	Make_HitCount();

	if (m_HitCount.empty())
	{
		Matrix matWorld = matHit * m_pTransformCom->Get_WorldMatrix();


		if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &matWorld)))
			return S_OK;

		m_pHitTexture->Set_SRV(m_pShaderCom, "g_DiffuseTexture");

		m_pShaderCom->Begin(0);

		m_pVIBufferCom->Render();

		return S_OK;
	}



	Vec3 vUIPos = -m_vOffsetDir * 45.0f;
	
	for (auto& Count : m_HitCount)
	{
		Matrix matWorld = matCount;
		matWorld.Translation(vUIPos);
		matWorld *= m_pTransformCom->Get_WorldMatrix();

		if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &matWorld)))
			return S_OK;

		m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", Count);

		m_pShaderCom->Begin(0);

		m_pVIBufferCom->Render();

		vUIPos += (m_vOffsetDir * 90.0f);
	}
	

	vUIPos += (m_vOffsetDir * 80.0f);
	vUIPos.y -= 30.0f;
	Matrix matWorld = matHits;
	matWorld.Translation(vUIPos);
	matWorld *= m_pTransformCom->Get_WorldMatrix();

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &matWorld)))
		return S_OK;

	m_pHitTexture->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 1);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();



	return S_OK;
}

void CUI_Hits::Appear()
{
	m_bActive = true;
	m_eState = UISTATE::APPEAR;


	m_fScale = 0.7f;
	

	m_pTransformCom->Set_Scale(Vec3(m_fScale, m_fScale, 1.f));

	m_fAlpha = 1.0f;

	m_fLifeTime = 3.0f;
}

void CUI_Hits::Add_Hit()
{
	++m_iHits;

	Appear();
}

void CUI_Hits::Make_HitCount()
{
	m_HitCount.clear();
	
	_uint iHits = m_iHits;

	if (iHits == 1)
		return;


	string strHits = to_string(m_iHits);
	_uint iCount = strHits.length();


	string strNumber;
	for (_uint i = 0; i < iCount; ++i)
	{
		strNumber.clear();
		strNumber = strHits[i];
		m_HitCount.push_back(stoi(strNumber));
	}
}

void CUI_Hits::UI_AppearTick(_float fTimeDelta)
{
	m_fScale += 10.0f * fTimeDelta;

	if (m_fScale > 1.0f)
		m_fScale = 1.0f;



	m_pTransformCom->Set_Scale(Vec3(m_fScale, m_fScale, 1.f));

	if (m_fScale == 1.0f)
	{
		m_eState = UISTATE::TICK;
	}


}

void CUI_Hits::UI_Tick(_float fTimeDelta)
{
	if (m_fScale > 0.7f)
		m_fScale -= 10.0f * fTimeDelta;

	m_pTransformCom->Set_Scale(Vec3(m_fScale, m_fScale, 1.f));


	m_fLifeTime -= fTimeDelta;

	if (m_fLifeTime <= 0.0f)
	{
		Disappear();
	}
}

void CUI_Hits::UI_DisappearTick(_float fTimeDelta)
{
	m_fAlpha -= 1.0f * fTimeDelta;

	if (m_fAlpha <= 0.0f)
	{
		m_fAlpha = 0.0f;
		m_iHits = 0;
		Set_Active(false);
	}
}



HRESULT CUI_Hits::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HitCount"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Hit"),
		TEXT("Com_HitTexture"), (CComponent**)&m_pHitTexture)))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Hits::Bind_ShaderResources()
{
	

	return S_OK;
}


CUI_Hits * CUI_Hits::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_Hits*	pInstance = new CUI_Hits(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : TitleCloud");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_Hits::Clone(void* pArg)
{
	CUI_Hits*	pInstance = new CUI_Hits(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : TitleCloud");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Hits::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
