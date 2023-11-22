#include "stdafx.h"
#include "RasenSyurikenBladeLine.h"
#include "Client_Defines.h"
#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
#include "Pool.h"
#include "RasenganCircle.h"
#include "Player_Naruto.h"



CRasenSyurikenBladeLine::CRasenSyurikenBladeLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CRasenSyurikenBladeLine::CRasenSyurikenBladeLine(const CRasenSyurikenBladeLine& rhs)
	: CEffect(rhs)
{
}

HRESULT CRasenSyurikenBladeLine::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CRasenSyurikenBladeLine::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	m_szModelName = L"Effect_Meteor_RasenSyurikenBladeLine";

	m_bActive = false;


	m_vColor = Vec4(0.03f, 0.69f, 0.94f, 0.5f);
	m_vBlur = m_vColor;

	//m_vBlur = Vec4(0.07f, 0.3f, 0.63f, 1.0f);

	m_vBlur *= 0.1f;


	m_fAlphaWeight = 1.0f;

	m_vTargetScale = Vec3(5.0f, 5.0f, 5.0f);

	
	m_fTurnSpeed = 10.0f;
	m_fScaleSpeed = 5.0f;


	return S_OK;
}

void CRasenSyurikenBladeLine::Tick(_float fTimeDelta)
{

	Update_Transform(fTimeDelta);
}

void CRasenSyurikenBladeLine::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CRasenSyurikenBladeLine::Render()
{
	
	if (nullptr == m_pModelCom || nullptr == m_pShaderCom)
		return S_OK;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return S_OK;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return S_OK;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return S_OK;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(Vec4))))
		return S_OK;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vBlurColor", &m_vBlur, sizeof(Vec4))))
		return S_OK;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaWeight", &m_fAlphaWeight, sizeof(_float))))
		return S_OK;


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;



	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{


		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 3)))
			return S_OK;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}



void CRasenSyurikenBladeLine::Appear()
{
	m_bActive = true;

	m_vColor.w = 0.7f;

	m_pTransformCom->Set_Scale(Vec3(0.01f, 0.01f, 0.01f));

	m_vCurrScale = Vec3(0.01f, 0.01f, 0.01f);

	m_fCurrRadian = 0.0f;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pOwner->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));
}

void CRasenSyurikenBladeLine::DisAppear()
{
	Set_Active(false);
}


HRESULT CRasenSyurikenBladeLine::Ready_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_LockFree_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_NoiseLine_BC"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	///* For.Com_Model */
	wstring strComName = L"Prototype_Component_Model_RasenSyurikenBladeLine";
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strComName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	Safe_Release(pGameInstance);



	return S_OK;
}

void CRasenSyurikenBladeLine::Update_Transform(_float fTimeDelta)
{
	
	if (m_vCurrScale != m_vTargetScale)
	{
		m_vCurrScale += Vec3(1.0f, 1.0f, 1.0f) * 5.0f * fTimeDelta;
		if (m_vCurrScale.x > m_vTargetScale.x)
			m_vCurrScale = m_vTargetScale;
	}

	Matrix matOwnerWorld = m_pOwner->Get_TransformCom()->Get_WorldMatrix();

	m_pTransformCom->Set_WorldMatrix(matOwnerWorld);
	m_pTransformCom->Set_Scale(m_vCurrScale);

	m_fCurrRadian += m_fTurnSpeed * fTimeDelta;

	Vec3 vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	m_pTransformCom->Turn(vUp, m_fCurrRadian);
}


CGameObject* CRasenSyurikenBladeLine::Clone(void* pArg)
{
	CRasenSyurikenBladeLine* pInstance = new CRasenSyurikenBladeLine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CRasenSyurikenBladeLine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRasenSyurikenBladeLine* CRasenSyurikenBladeLine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRasenSyurikenBladeLine* pInstance = new CRasenSyurikenBladeLine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRasenSyurikenBladeLine");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CRasenSyurikenBladeLine::Free()
{
	__super::Free();
}
