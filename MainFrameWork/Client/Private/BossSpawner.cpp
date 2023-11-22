#include "stdafx.h"
#include "GameInstance.h"
#include "BossSpawner.h"
#include "AsUtils.h"
#include "CollisionManager.h"
#include "RigidBody.h"
#include "NavigationMgr.h"
#include "ColliderSphere.h"



CBossSpawner::CBossSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext, L"BossSpawner", OBJ_TYPE::SPAWNER)
{
}

CBossSpawner::CBossSpawner(const CBossSpawner& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBossSpawner::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBossSpawner::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	_uint* pObjectID = (_uint*)pArg;
	m_iObjectID = *pObjectID;

	m_vColor = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
	m_vBlurColor = Vec4(0.68f, 0.0f, 0.0f, 1.0f);
	//m_vColor = Vec4(0.035f, 0.69f, 0.95f, 1.0f);
	//m_vBlurColor = Vec4(0.074f, 0.24f, 0.58f, 1.0f);

	m_fTurnSpeed = 0.4f;


	m_bEnter = false;


	m_vTargetScale = Vec3(32.0f, 1.0f, 32.0f);

    return S_OK;
}

void CBossSpawner::Tick(_float fTimeDelta)
{
	m_pTransformCom->Turn_Speed(Vec3(0.0f, 1.0f, 0.0f), m_fTurnSpeed, fTimeDelta);


	if (m_bEnter)
	{
		Vec3 vScale = m_pTransformCom->Get_Scale();

		vScale = Vec3::Lerp(vScale, m_vTargetScale, fTimeDelta);

		m_pTransformCom->Set_Scale(vScale);

		m_vColor.w -= 2.0f * fTimeDelta;

		if (m_vColor.w <= 0.0f)
		{
			m_vColor.w = 0.0f;
			Set_Active(false);
		}
	}


}

void CBossSpawner::LateTick(_float fTimeDelta)
{
	if (m_bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CBossSpawner::Render()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vBlurColor", &m_vBlurColor, sizeof(Vec4))))
		return S_OK;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return S_OK;

		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 2)))
			return S_OK;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

void CBossSpawner::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	m_bEnter = true;
}

void CBossSpawner::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
}

void CBossSpawner::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
	
}





HRESULT CBossSpawner::Ready_Components()
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


	///* For.Com_Model */
	wstring strComName = L"Prototype_Component_Model_AreaCircle";
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strComName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;




	Safe_Release(pGameInstance);



	Vec3 vScale;
	vScale.x = 8.0f;
	vScale.y = 1.0f;
	vScale.z = 8.0f;

	m_pTransformCom->Set_Scale(vScale);

    return S_OK;
}



void CBossSpawner::Set_Die()
{
	for (auto& Collider : m_Coliders)
		Collider.second->SetActive(false);

	m_bDie = true;
}





void CBossSpawner::Free()
{
	__super::Free();



	Safe_Release(m_pModelCom);

	Safe_Release(m_pTransformCom);
}

CBossSpawner* CBossSpawner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossSpawner* pInstance = new CBossSpawner(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		cout << "Failed To Created : CMonsterSpawner" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBossSpawner::Clone(void* pArg)
{
	CBossSpawner* pInstance = new CBossSpawner(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CMonsterSpawner" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}
