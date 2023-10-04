#include "stdafx.h"
#include "ServerPacketHandler.h"
#include "GameSessionManager.h"
#include "GameSession.h"
#include "GameInstance.h"
#include "Player_Server.h"
#include "AsUtils.h"
#include "Struct.pb.h"
#include "Protocol.pb.h"
#include "ColliderSphere.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID_Server(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_S_TIME_Server(PacketSessionRef& session, Protocol::S_TIME& pkt)
{
	CGameSessionManager::TIME tServerTime = CGameSessionManager::GetInstance()->Get_ServerTime();

	pkt.set_iserverminute(tServerTime.iMinute);
	pkt.set_fserversecond(tServerTime.fSecond);
	SendBufferRef pSendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);

	session->Send(pSendBuffer);

	return true;
}

bool Handel_S_OPENLEVEL_Server(PacketSessionRef& session, Protocol::S_OPEN_LEVEL& pkt)
{
	return true;
}

bool Handel_S_LEVELSTATE_Server(PacketSessionRef& session, Protocol::S_LEVEL_STATE& pkt)
{
	shared_ptr<CGameSession> pGameSession = dynamic_pointer_cast<CGameSession>(session);
	pGameSession->Set_LevelState((LEVELSTATE)pkt.ilevelstate());
	return true;
}

bool Handel_S_CREATEOBJECT_Server(PacketSessionRef& session, Protocol::S_CREATE_OBJCECT& pkt)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/*switch ((OBJ_TYPE)pkt.iobjecttype())
	{
	case OBJ_TYPE::PLAYER:
	{
		CPlayer_Server::MODELDESC Desc;
		Desc.strFileName = CAsUtils::ToWString(pkt.strname());

		if (nullptr == pGameInstance->Add_GameObject(pkt.ilevel(), pkt.ilayer(), TEXT("Prototype_GameObject_Player"), &Desc))
			return E_FAIL;

		break;
	}
	case OBJ_TYPE::MONSTER:
		break;
	case OBJ_TYPE::PROP:
		break;
	}*/

	Safe_Release(pGameInstance);

	return true;
}

bool Handel_S_CHARACTERNAME_Server(PacketSessionRef& session, Protocol::S_CHARACTER_NAME& pkt)
{
	shared_ptr<CGameSession> pGameSession = dynamic_pointer_cast<CGameSession>(session);
	pGameSession->Set_CharacterName(CAsUtils::ToWString(pkt.strname()));
	return true;
}

bool Handel_S_MATRIX_Server(PacketSessionRef& session, Protocol::S_MATRIX& pkt)
{
	SendBufferRef pBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
	CGameSessionManager::GetInstance()->Broadcast_Others(pBuffer, session->GetSessionID());


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CGameObject* pObject = pGameInstance->Find_GameObejct(pkt.ilevel(), pkt.ilayer(), pkt.iobjectid());
	CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Get_Component(L"Com_Transform"));

	Matrix matWorld;

	memcpy(&matWorld.m[0], &pkt.matrix()[0], sizeof(_float) * 16);


	pTransform->Set_WorldMatrix(matWorld);
	Safe_Release(pGameInstance);

	return true;
}

bool Handel_S_ANIMATION_Server(PacketSessionRef& session, Protocol::S_ANIMATION& pkt)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CGameObject* pObject = pGameInstance->Find_GameObejct(pkt.ilevel(), pkt.ilayer(), pkt.iobjectid());

	if (pObject == nullptr)
	{
		Safe_Release(pGameInstance);
		return true;
	}


	CModel* pModel = dynamic_cast<CModel*>(pObject->Get_Component(L"Com_Model"));

	pModel->Reserve_NextAnimation(pkt.ianimindex(), pkt.fchangetime(), pkt.istartframe(), pkt.ichangeframe());

	Safe_Release(pGameInstance);


	return true;
}

bool Handel_S_PLAYERINFO_Server(PacketSessionRef& session, Protocol::S_PLAYERINFO& pkt)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	auto tPlayer = pkt.mutable_tplayer(0);


	CPlayer_Server* pPlayer = dynamic_cast<CPlayer_Server*>(pGameInstance->Find_GameObejct(tPlayer->ilevel(), tPlayer->ilayer(), tPlayer->iplayerid()));
	if (pPlayer == nullptr)
		return true;


	pPlayer->Set_TargetPos(Vec3(tPlayer->mutable_vtargetpos()->mutable_data()));
	pPlayer->Get_TransformCom()->Set_WorldMatrix(Matrix(tPlayer->mutable_matworld()->mutable_data()));


	CGameSessionManager::GetInstance()->Add_SendCount();
	cout << CGameSessionManager::GetInstance()->Get_SendCount() << endl;

	Safe_Release(pGameInstance);

	return true;
}

bool Handel_S_STATE_Server(PacketSessionRef& session, Protocol::S_STATE& pkt)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	auto tPlayer = pkt.tplayer();
	
	CPlayer_Server* pPlayer = dynamic_cast<CPlayer_Server*>(pGameInstance->Find_GameObejct(tPlayer.ilevel(), tPlayer.ilayer(), tPlayer.iplayerid()));
	if (pPlayer == nullptr)
		return true;

	pPlayer->Get_TransformCom()->Set_WorldMatrix(Matrix(tPlayer.matworld().data()));
	pPlayer->Set_TargetPos(Vec3(tPlayer.vtargetpos().data()));

	SendBufferRef pBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
	CGameSessionManager::GetInstance()->Broadcast_Others(pBuffer, session->GetSessionID());

	Safe_Release(pGameInstance);

	return true;
}

bool Handel_S_COLLIDERSTATE_Server(PacketSessionRef& session, Protocol::S_COLLIDERSTATE& pkt)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	CGameObject* pObject = pGameInstance->Find_GameObejct(pkt.ilevel(), pkt.ilayer(), pkt.iobjectid());
	if (pObject == nullptr)
		return true;


	CSphereCollider* pCollider = pObject->Get_Colider(pkt.icollayer());

	pCollider->SetActive(pkt.bactive());
	pCollider->Set_Radius(pkt.fradius());
	pCollider->Set_BoneIndex(pkt.iboneindex());

	Safe_Release(pGameInstance);
	return true;
}

bool Handel_S_COLLISION_Server(PacketSessionRef& session, Protocol::S_COLLISION& pkt)
{

	return true;
}
