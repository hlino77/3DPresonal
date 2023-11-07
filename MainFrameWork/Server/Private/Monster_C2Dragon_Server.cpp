#include "stdafx.h"
#include "GameInstance.h"
#include "Monster_C2Dragon_Server.h"
#include "AsUtils.h"
#include "ColliderSphere.h"
#include "GameSessionManager.h"
#include "Boss_Deidara_Server.h"
#include "State_C2Dragon_Idle_Server.h"
#include "State_C2Dragon_Attack_Server.h"
#include "Skill_Hiryu_Server.h"

CMonster_C2Dragon_Server::CMonster_C2Dragon_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster_Server(pDevice, pContext)
{
}

CMonster_C2Dragon_Server::CMonster_C2Dragon_Server(const CMonster_C2Dragon_Server& rhs)
	: CMonster_Server(rhs)
{
}

HRESULT CMonster_C2Dragon_Server::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster_C2Dragon_Server::Initialize(void* pArg)
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

void CMonster_C2Dragon_Server::Tick(_float fTimeDelta)
{
	m_pStateMachine->Tick_State(fTimeDelta);


	m_fSpawnTime -= fTimeDelta;

	if (m_fSpawnTime <= 0.0f)
	{
		Set_Active(false);
	}
}

void CMonster_C2Dragon_Server::LateTick(_float fTimeDelta)
{
	m_PlayAnimation = std::async(&CModel::Play_Animation, m_pModelCom, fTimeDelta);

	if (m_bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_fSendInfoTime += fTimeDelta;
	if (m_fSendInfoTime >= 0.05f)
	{
		m_fSendInfoTime = 0.0f;
		Send_MonsterInfo();
	}
}

HRESULT CMonster_C2Dragon_Server::Render()
{
	__super::Render();

	return S_OK;
}

void CMonster_C2Dragon_Server::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	
}

void CMonster_C2Dragon_Server::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
	
}

void CMonster_C2Dragon_Server::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{

}

void CMonster_C2Dragon_Server::Set_Skill(CGameObject* pObject)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Send_MakeSkill(L"Hiryu", (CGameObject**)&m_pHiryu);

	Safe_Release(pGameInstance);
}

void CMonster_C2Dragon_Server::Send_MonsterInfo()
{
	Protocol::S_OBJECTINFO pkt;

	auto tObject = pkt.add_tobject();

	tObject->set_iobjectid(m_iObjectID);
	tObject->set_ilevel(CGameInstance::GetInstance()->Get_CurrLevelIndex());
	tObject->set_ilayer((_uint)LAYER_TYPE::LAYER_MONSTER);


	auto vTargetPos = tObject->mutable_vtargetpos();
	vTargetPos->Resize(3, 0.0f);
	Vec3 vPlayerTargetPos = m_vTargetPos.load();
	memcpy(vTargetPos->mutable_data(), &vPlayerTargetPos, sizeof(Vec3));


	auto matWorld = tObject->mutable_matworld();
	matWorld->Resize(16, 0.0f);
	Matrix matPlayerWorld = m_pTransformCom->Get_WorldMatrix();
	memcpy(matWorld->mutable_data(), &matPlayerWorld, sizeof(Matrix));


	SendBufferRef pSendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
	CGameSessionManager::GetInstance()->Broadcast(pSendBuffer);
}


void CMonster_C2Dragon_Server::Update_NearTarget(_float fTimeDelta)
{
	m_pNearTarget = CGameInstance::GetInstance()->Find_NearGameObject(CGameInstance::GetInstance()->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_PLAYER, this);
}


void CMonster_C2Dragon_Server::Spawn()
{
	Set_Active(true);
	Set_State(L"Idle");

	m_fAttackDelay = 0.0f;
	m_fSpawnTime = 32.0f;
}

void CMonster_C2Dragon_Server::Shoot_Hiryu()
{
	Matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	Matrix matBone = m_pModelCom->Get_CurrBoneMatrix(m_BoneIndex[L"Hiryu"]);

	Matrix matHiryu = matBone * matWorld;


	Vec3 vPos(matHiryu.m[3]);
	Vec3 vLook(matHiryu.m[2]);
	vLook.Normalize();

	m_pHiryu->Shoot_Hiryu(m_pNearTarget, vPos, vLook);
}

HRESULT CMonster_C2Dragon_Server::Ready_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
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

void CMonster_C2Dragon_Server::Ready_State()
{
	m_pStateMachine->Add_State(L"Idle", new CState_C2Dragon_Idle_Server(L"Idle", this));
	m_pStateMachine->Add_State(L"Attack", new CState_C2Dragon_Attack_Server(L"Attack", this));

	m_pStateMachine->Change_State(L"Idle");
}

void CMonster_C2Dragon_Server::Set_ToDeidara()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	CGameObject* pObject = pGameInstance->Find_GameObejct(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_BOSS, L"Deidara");
	
	if (pObject)
		dynamic_cast<CBoss_Deidara_Server*>(pObject)->Set_C2Dragon(this);


	Safe_Release(pGameInstance);
}

void CMonster_C2Dragon_Server::Set_Die()
{
	m_pHiryu->Set_Die();

	Set_Active(false);
}

CMonster_C2Dragon_Server* CMonster_C2Dragon_Server::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_C2Dragon_Server* pInstance = new CMonster_C2Dragon_Server(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		cout << "Failed To Created : CMonster_C2Dragon_Server" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_C2Dragon_Server::Clone(void* pArg)
{
	CMonster_C2Dragon_Server* pInstance = new CMonster_C2Dragon_Server(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CMonster_C2Dragon_Server" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_C2Dragon_Server::Free()
{
	__super::Free();
}
