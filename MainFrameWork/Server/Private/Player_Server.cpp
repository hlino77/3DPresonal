#include "stdafx.h"
#include "GameInstance.h"
#include "Player_Server.h"
#include "Key_Manager.h"
#include "ColliderSphere.h"
#include "CollisionManager.h"
#include "ColliderSphere.h"
#include "GameSession.h"


CPlayer_Server::CPlayer_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext, L"Player", OBJ_TYPE::PLAYER),
	m_vTargetPos(Vec3(0.0f, 0.0f, 0.0f))
{
}

CPlayer_Server::CPlayer_Server(const CPlayer_Server& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer_Server::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer_Server::Initialize(void* pArg)
{
	MODELDESC* Desc = static_cast<MODELDESC*>(pArg);
	m_strObjectTag = Desc->strFileName;
	m_iObjectID = Desc->iObjectID;
	m_iLayer = Desc->iLayer;
	m_pGameSession = Desc->pGameSession;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Sockets()))
		return E_FAIL;

	if (FAILED(Ready_PlayerParts()))
		return E_FAIL;


    return S_OK;
}

void CPlayer_Server::Tick(_float fTimeDelta)
{
}

void CPlayer_Server::LateTick(_float fTimeDelta)
{
	//m_pModelCom->Play_Animation(fTimeDelta);

	Set_Colliders();
}

HRESULT CPlayer_Server::Render()
{


    return S_OK;
}

void CPlayer_Server::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->AddRef();


	Protocol::S_COLLISION pkt;

	pkt.set_benter(true);

	pkt.set_ilevel(pGameInstance->Get_CurrLevelIndex());
	pkt.set_ilayer((_uint)LAYER_TYPE::LAYER_PLAYER);
	pkt.set_iobjectid(m_iObjectID);
	pkt.set_icollayer(iColLayer);


	CGameObject* pOtherObject = pOther->Get_Owner();
	pkt.set_iotherid(pOtherObject->Get_ObjectID());
	pkt.set_iotherlayer(pOtherObject->Get_ObjectLayer());
	pkt.set_iothercollayer(pOther->Get_ColLayer());

	SendBufferRef pSendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
	m_pGameSession->Send(pSendBuffer);

	Safe_Release(pGameInstance);
}

void CPlayer_Server::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
}

void CPlayer_Server::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->AddRef();


	Protocol::S_COLLISION pkt;

	pkt.set_benter(false);

	pkt.set_ilevel(pGameInstance->Get_CurrLevelIndex());
	pkt.set_ilayer((_uint)LAYER_TYPE::LAYER_PLAYER);
	pkt.set_iobjectid(m_iObjectID);
	pkt.set_icollayer(iColLayer);


	CGameObject* pOtherObject = pOther->Get_Owner();
	pkt.set_iotherid(pOtherObject->Get_ObjectID());
	pkt.set_iotherlayer(pOtherObject->Get_ObjectLayer());
	pkt.set_iothercollayer(pOther->Get_ColLayer());

	SendBufferRef pSendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
	m_pGameSession->Send(pSendBuffer);

	Safe_Release(pGameInstance);
}

void CPlayer_Server::Set_Colliders()
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);
	Vec3 vUp = m_pTransformCom->Get_State(CTransform::STATE::STATE_UP);
	vUp.Normalize();

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Center(vPos + vUp * 0.7f);
}

HRESULT CPlayer_Server::Ready_Components()
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

HRESULT CPlayer_Server::Ready_Sockets()
{
	/*if (nullptr == m_pModelCom)
		return E_FAIL;

	CHierarchyNode* pWeaponSocket = m_pModelCom->Get_HierarchyNode("SWORD");
	if (nullptr == pWeaponSocket)
		return E_FAIL;

	m_Sockets.push_back(pWeaponSocket);*/

	return S_OK;
}

HRESULT CPlayer_Server::Ready_PlayerParts()
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

HRESULT CPlayer_Server::Update_Weapon()
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

CPlayer_Server* CPlayer_Server::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Server* pInstance = new CPlayer_Server(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Server::Clone(void* pArg)
{
	CPlayer_Server* pInstance = new CPlayer_Server(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Server::Free()
{
	__super::Free();

	for (auto& pPart : m_Parts)
		Safe_Release(pPart);

	m_Parts.clear();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
