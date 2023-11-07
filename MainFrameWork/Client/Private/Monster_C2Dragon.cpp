#include "stdafx.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Monster_C2Dragon.h"
#include "Key_Manager.h"
#include "Camera_Player.h"
#include "ServerSessionManager.h"
#include "Engine_Defines.h"
#include "ColliderSphere.h"
#include "State_C2Dragon_Idle.h"
#include "Boss_Deidara.h"
#include "State_C2Dragon_Attack.h"
#include "Skill_Hiryu.h"

CMonster_C2Dragon::CMonster_C2Dragon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

CMonster_C2Dragon::CMonster_C2Dragon(const CMonster_C2Dragon& rhs)
	: CMonster(rhs)
{
}

HRESULT CMonster_C2Dragon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_C2Dragon::Initialize(void* pArg)
{
	MODELDESC* Desc = static_cast<MODELDESC*>(pArg);
	m_strObjectTag = Desc->strFileName;
	m_iObjectID = Desc->iObjectID;
	m_iLayer = Desc->iLayer;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	Ready_State();



	Set_ToDeidara();


	m_BoneIndex.emplace(L"Hiryu", m_pModelCom->Find_BoneIndex(L"kuti_T_end"));

	Set_Active(false);

	return S_OK;
}

void CMonster_C2Dragon::Tick(_float fTimeDelta)
{
	m_pStateMachine->Tick_State(fTimeDelta);

	

	m_fSpawnTime -= fTimeDelta;
	if (m_fSpawnTime <= 0.0f)
	{
		Set_Active(false);
	}
}

void CMonster_C2Dragon::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

}

HRESULT CMonster_C2Dragon::Render()
{
	__super::Render();

	return S_OK;
}

void CMonster_C2Dragon::Set_Skill(CGameObject* pGameObject)
{
	m_pHiryu = dynamic_cast<CSkill_Hiryu*>(pGameObject);
}

void CMonster_C2Dragon::Set_Colliders()
{


}



void CMonster_C2Dragon::Spawn()
{
	Set_Active(true);

	m_fSpawnTime = 32.0f;
}

void CMonster_C2Dragon::Shoot_Hiryu()
{
	Matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	Matrix matBone = m_pModelCom->Get_CurrBoneMatrix(m_BoneIndex[L"Hiryu"]);

	Matrix matHiryu = matBone * matWorld;


	Vec3 vPos(matHiryu.m[3]);
	Vec3 vLook(matHiryu.m[2]);
	vLook.Normalize();

	m_pHiryu->Shoot_Hiryu(m_pNearTarget, vPos, vLook);
}

void CMonster_C2Dragon::Set_Die()
{
	m_pHiryu->Set_Die();
	Set_Active(false);
}

HRESULT CMonster_C2Dragon::Ready_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	///* For.Com_State */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachine)))
		return E_FAIL;

	///* For.Com_Model */
	wstring strComName = L"Prototype_Component_Model_" + m_strObjectTag;
	if (FAILED(__super::Add_Component(pGameInstance->Get_CurrLevelIndex(), strComName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	Safe_Release(pGameInstance);

	Vec3 vScale;
	vScale.x = 0.01f;
	vScale.y = 0.01f;
	vScale.z = 0.01f;

	m_pTransformCom->Set_Scale(vScale);

	return S_OK;
}

HRESULT CMonster_C2Dragon::Ready_State()
{
	m_pStateMachine->Add_State(L"Idle", new CState_C2Dragon_Idle(L"Idle", this));
	m_pStateMachine->Add_State(L"Attack", new CState_C2Dragon_Attack(L"Attack", this));


	m_pStateMachine->Change_State(L"Idle");
	return S_OK;
}

void CMonster_C2Dragon::Set_ToDeidara()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CGameObject* pObject = pGameInstance->Find_GameObejct(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_BOSS, L"Deidara");

	if (pObject)
		dynamic_cast<CBoss_Deidara*>(pObject)->Set_C2Dragon(this);


	Safe_Release(pGameInstance);
}


CMonster_C2Dragon* CMonster_C2Dragon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_C2Dragon* pInstance = new CMonster_C2Dragon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CMonster_C2Dragon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_C2Dragon::Clone(void* pArg)
{
	CMonster_C2Dragon* pInstance = new CMonster_C2Dragon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CMonster_C2Dragon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_C2Dragon::Free()
{
	__super::Free();
}
