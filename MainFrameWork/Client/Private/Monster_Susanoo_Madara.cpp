#include "stdafx.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Monster_Susanoo_Madara.h"
#include "Engine_Defines.h"

CMonster_Susanoo_Madara::CMonster_Susanoo_Madara(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

CMonster_Susanoo_Madara::CMonster_Susanoo_Madara(const CMonster_Susanoo_Madara& rhs)
	: CMonster(rhs)
{
}

HRESULT CMonster_Susanoo_Madara::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_Susanoo_Madara::Initialize(void* pArg)
{
	m_strObjectTag = L"Susanoo_Madara";
	m_iObjectID = -100;
	m_iLayer = (_uint)LAYER_TYPE::LAYER_EFFECT;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Set_Active(false);

	m_vBlurColor = Vec4(0.1f, 0.1f, 0.99f, 1.0f);

	m_vBlurColor *= 0.3f;
	//m_vBlurColor += Vec4(1.0f, 1.0f, 1.0f, 0.0f) * 0.5;
	

	return S_OK;
}

void CMonster_Susanoo_Madara::Tick(_float fTimeDelta)
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	vPos = Vec3::Lerp(vPos, m_vTargetPos, 2.0f * fTimeDelta);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);


	if (m_bEnd)
	{
		m_fAlpha -= fTimeDelta;

		if (m_fAlpha <= 0.0f)
			Set_Active(false);
	}


	_uint iFrame = m_pModelCom->Get_Anim_Frame(0);
	if (iFrame > 95)
		m_bEnd = true;
}

void CMonster_Susanoo_Madara::LateTick(_float fTimeDelta)
{
	if(m_bEnd == false)
		m_PlayAnimation = std::async(&CModel::Play_Animation, m_pModelCom, fTimeDelta * m_fAnimationSpeed);

	if (nullptr == m_pRendererCom)
		return;

	if (m_bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CMonster_Susanoo_Madara::Render()
{
	if (nullptr == m_pModelCom || nullptr == m_pShaderCom)
		return S_OK;

	if(m_bEnd == false)
		m_PlayAnimation.get();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return S_OK;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return S_OK;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return S_OK;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vBlurColor", &m_vBlurColor, sizeof(Vec4))))
		return S_OK;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return S_OK;


	m_pModelCom->SetUpAnimation_OnShader(m_pShaderCom);


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return S_OK;

		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 1)))
			return S_OK;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}


void CMonster_Susanoo_Madara::Set_Colliders()
{


}

void CMonster_Susanoo_Madara::Appear(Vec3 vPos, Vec3 vLook)
{
	m_vTargetPos = vPos;

	vPos.y -= 10.0f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->LookAt_ForLandObject(vPos + vLook);

	m_pModelCom->Set_CurrAnim(0);

	m_fAlpha = 1.0f;
	m_bEnd = false;


	Set_Active(true);

	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"MadaraSusanoo.wav", g_fVolume * 0.8f, vPos, 40.0f);
}



HRESULT CMonster_Susanoo_Madara::Ready_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_UseLock_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;



	///* For.Com_Model */
	wstring strComName = L"Prototype_Component_Model_Susanoo_Madara";
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strComName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	Safe_Release(pGameInstance);

	Vec3 vScale;
	vScale.x = 0.01f;
	vScale.y = 0.01f;
	vScale.z = 0.01f;

	m_pTransformCom->Set_Scale(vScale);

	return S_OK;
}




CMonster_Susanoo_Madara* CMonster_Susanoo_Madara::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_Susanoo_Madara* pInstance = new CMonster_Susanoo_Madara(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CMonster_Susanoo_Madara");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_Susanoo_Madara::Clone(void* pArg)
{
	CMonster_Susanoo_Madara* pInstance = new CMonster_Susanoo_Madara(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CMonster_Susanoo_Madara");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Susanoo_Madara::Free()
{
	__super::Free();
}
