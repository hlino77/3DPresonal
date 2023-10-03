#include "stdafx.h"
#include "GameInstance.h"
#include "Monster_Server.h"
#include "AsUtils.h"
#include "ColliderSphere.h"
#include "GameSessionManager.h"
#include "CollisionManager.h"


CMonster_Server::CMonster_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext, L"Monster", OBJ_TYPE::MONSTER),
	m_vTargetPos(Vec3(0.0f, 0.0f, 0.0f))
{
}

CMonster_Server::CMonster_Server(const CMonster_Server& rhs)
	: CGameObject(rhs),
	m_matTargetWorld(rhs.m_matTargetWorld.load())
{
}

HRESULT CMonster_Server::Initialize_Prototype()
{
	m_matTargetWorld = XMMatrixIdentity();
    return S_OK;
}

HRESULT CMonster_Server::Initialize(void* pArg)
{
	MODELDESC* Desc = static_cast<MODELDESC*>(pArg);
	m_strObjectTag = Desc->strFileName;
	m_iObjectID = Desc->iObjectID;
	m_iLayer = Desc->iLayer;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Sockets()))
		return E_FAIL;

	if (FAILED(Ready_PlayerParts()))
		return E_FAIL;

    return S_OK;
}

void CMonster_Server::Tick(_float fTimeDelta)
{
}

void CMonster_Server::LateTick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
	Set_Colliders();
}

HRESULT CMonster_Server::Render()
{
	return S_OK;
}

void CMonster_Server::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
}

void CMonster_Server::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
}

void CMonster_Server::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
}


HRESULT CMonster_Server::Ready_Components()
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

	///* For.Com_State */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachine)))
		return E_FAIL;


	///* For.Com_Model */
	wstring strComName = L"Prototype_Component_Model_" + m_strObjectTag;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, strComName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	{
		CCollider::ColliderInfo tColliderInfo;
		tColliderInfo.m_bActive = true;
		tColliderInfo.m_iLayer = (_uint)LAYER_COLLIDER::LAYER_BODY;
		tColliderInfo.pOwner = this;
		CSphereCollider* pCollider = nullptr;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_SphereColider"), TEXT("Com_SphereColider"), (CComponent**)&pCollider, &tColliderInfo)))
			return E_FAIL;

		m_Coliders.emplace((_uint)LAYER_COLLIDER::LAYER_BODY, pCollider);
		CCollisionManager::GetInstance()->Add_Colider(pCollider);
	}


	Safe_Release(pGameInstance);

	Vec3 vScale;
	vScale.x = 0.01f;
	vScale.y = 0.01f;
	vScale.z = 0.01f;

	m_pTransformCom->Set_Scale(vScale);

    return S_OK;
}


void CMonster_Server::Send_State(const wstring& szName)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Protocol::S_STATE pkt;
	pkt.set_strstate(CAsUtils::ToString(szName));

	auto tPlayer = pkt.mutable_tplayer();

	tPlayer->set_ilevel(pGameInstance->Get_CurrLevelIndex());
	tPlayer->set_ilayer((_uint)LAYER_TYPE::LAYER_PLAYER);
	tPlayer->set_iplayerid(m_iObjectID);

	auto vTargetPos = tPlayer->mutable_vtargetpos();
	vTargetPos->Resize(3, 0.0f);
	Vec3 vPlayerTargetPos = m_vTargetPos.load();
	memcpy(vTargetPos->mutable_data(), &vPlayerTargetPos, sizeof(Vec3));


	auto matWorld = tPlayer->mutable_matworld();
	matWorld->Resize(16, 0.0f);
	Matrix matPlayerWorld = m_pTransformCom->Get_WorldMatrix();
	memcpy(matWorld->mutable_data(), &matPlayerWorld, sizeof(Matrix));


	SendBufferRef pSendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
	CGameSessionManager::GetInstance()->Broadcast(pSendBuffer);

	Safe_Release(pGameInstance);
}


void CMonster_Server::Set_State(const wstring& szName)
{
	m_pStateMachine->Change_State(szName);
	Send_State(szName);
}

void CMonster_Server::Set_NoneControlState(const wstring& szName)
{
	m_pStateMachine->Change_State(szName);
}

void CMonster_Server::Reserve_Animation(_uint iAnimIndex, _float fChangeTime, _uint iStartFrame, _uint iChangeFrame)
{
	m_pModelCom->Reserve_NextAnimation(iAnimIndex, fChangeTime, iStartFrame, iChangeFrame);
}

void CMonster_Server::Set_Colliders()
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);
	Vec3 vUp = m_pTransformCom->Get_State(CTransform::STATE::STATE_UP);
	vUp.Normalize();

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Center(vPos + vUp * 0.7f);
}



HRESULT CMonster_Server::Ready_Sockets()
{
	/*if (nullptr == m_pModelCom)
		return E_FAIL;

	CHierarchyNode* pWeaponSocket = m_pModelCom->Get_HierarchyNode("SWORD");
	if (nullptr == pWeaponSocket)
		return E_FAIL;

	m_Sockets.push_back(pWeaponSocket);*/

	return S_OK;
}

HRESULT CMonster_Server::Ready_PlayerParts()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Sword */
	//CGameObject* pGameObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Sword"));

	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//m_Parts.push_back(pGameObject);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CMonster_Server::Update_Weapon()
{
	//if (nullptr == m_Sockets[PART_WEAPON])
	//	return E_FAIL;

	///* 행렬. */
	///*_matrix			WeaponMatrix = 뼈의 스페이스 변환(OffsetMatrix)
	//	* 뼈의 행렬(CombinedTransformation)
	//	* 모델의 PivotMatrix * 프렐이어의월드행렬. ;*/

	//_matrix WeaponMatrix = m_Sockets[PART_WEAPON]->Get_OffSetMatrix()
	//	* m_Sockets[PART_WEAPON]->Get_CombinedTransformation()
	//	* m_pModelCom->Get_PivotMatrix()
	//	* m_pTransformCom->Get_WorldMatrix();

	//m_Parts[PART_WEAPON]->SetUp_State(WeaponMatrix);

	return S_OK;
}


CGameObject* CMonster_Server::Clone(void* pArg)
{
	CMonster_Server* pInstance = new CMonster_Server(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CMonster" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Server::Free()
{
	__super::Free();



	Safe_Release(m_pModelCom);

	Safe_Release(m_pTransformCom);
}
