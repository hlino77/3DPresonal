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
#include "Level_Ready_Server.h"
#include "LobbyUser_Server.h"
#include "ColliderOBB.h"

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

bool Handel_S_OBJECTINFO_Server(PacketSessionRef& session, Protocol::S_OBJECTINFO& pkt)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	auto tObject = pkt.mutable_tobject(0);


	CGameObject* pObject = pGameInstance->Find_GameObejct(tObject->ilevel(), tObject->ilayer(), tObject->iobjectid());
	if (pObject == nullptr)
	{
		Safe_Release(pGameInstance);
		return true;
	}


	pObject->Set_TargetPos(Vec3(tObject->mutable_vtargetpos()->mutable_data()));
	pObject->Get_TransformCom()->Set_WorldMatrix(Matrix(tObject->mutable_matworld()->mutable_data()));


	//CGameSessionManager::GetInstance()->Add_SendCount();
	//cout << CGameSessionManager::GetInstance()->Get_SendCount() << endl;

	Safe_Release(pGameInstance);
	return true;
}

bool Handel_S_STATE_Server(PacketSessionRef& session, Protocol::S_STATE& pkt)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	auto tObject = pkt.tobject();
	
	

	CGameObject* pObject = pGameInstance->Find_GameObejct(tObject.ilevel(), tObject.ilayer(), tObject.iobjectid());
	if (pObject == nullptr)
	{
		Safe_Release(pGameInstance);
		return true;
	}


	Vec3 vTargetPos(tObject.vtargetpos().data());
	Matrix matTargetWorld(tObject.matworld().data());


	pObject->Set_TargetPos(vTargetPos);
	pObject->Set_TargetMatrix(matTargetWorld);


	if (pkt.itargetobjectid() == -1)
		pObject->Reset_NearTarget();
	else
	{
		CGameObject* pNearTarget = pGameInstance->Find_GameObejct(tObject.ilevel(), pkt.itargetobjectlayer(), pkt.itargetobjectid());
		if (pNearTarget == nullptr)
		{
			Safe_Release(pGameInstance);
			return true;
		}
		pObject->Set_NearTarget(pNearTarget);
	}


	if (pkt.ihitobjectid() == -1)
		pObject->Reset_HitObject();
	else
	{
		CGameObject* pHitObject = pGameInstance->Find_GameObejct(tObject.ilevel(), pkt.ihitobjectlayer(), pkt.ihitobjectid());
		if (pHitObject == nullptr)
		{
			Safe_Release(pGameInstance);
			return true;
		}
		pObject->Set_HitObject(pHitObject);
	}


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
	{
		Safe_Release(pGameInstance);
		return true;
	}


	CSphereCollider* pCollider = pObject->Get_Colider(pkt.icollayer());

	pCollider->SetActive(pkt.bactive());
	pCollider->Set_Radius(pkt.fradius());
	pCollider->Set_Offset(Vec3(pkt.voffset().data()));
	pCollider->Set_AttackType(pkt.iattacktype());
	pCollider->Set_Center();


	if (pkt.tchild_size() > 0)
	{
		auto& tChild = pkt.tchild(0);
		COBBCollider* pChild = dynamic_cast<COBBCollider*>(pCollider->Get_Child());
		
		pChild->Set_Offset(Vec3(tChild.voffset().data()));
		pChild->Set_Scale(Vec3(tChild.vscale().data()));
	}

	Safe_Release(pGameInstance);
	return true;
}

bool Handel_S_COLLISION_Server(PacketSessionRef& session, Protocol::S_COLLISION& pkt)
{

	return true;
}

bool Handel_S_NICKNAME_Server(PacketSessionRef& session, Protocol::S_NICKNAME& pkt)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	shared_ptr<CGameSession> pGameSession = dynamic_pointer_cast<CGameSession>(session);
	pGameSession->Set_NickName(CAsUtils::ToWString(pkt.strnickname()));

	
	if (pGameInstance->Get_CurrLevelIndex() != LEVELID::LEVEL_READY)
	{
		Safe_Release(pGameInstance);
		return true;
	}

	CLevel_Ready_Server* pLevel = dynamic_cast<CLevel_Ready_Server*>(pGameInstance->Get_CurrLevel());
	
	pLevel->Add_LobbyUser(pGameSession->Get_NickName());

	Safe_Release(pGameInstance);

	return true;
}

bool Handel_S_USERINFO_Server(PacketSessionRef& session, Protocol::S_USERINFO& pkt)
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_CurrLevelIndex() != LEVELID::LEVEL_READY)
	{
		Safe_Release(pGameInstance);
		return true;
	}


	CLevel_Ready_Server* pLevel = dynamic_cast<CLevel_Ready_Server*>(pGameInstance->Get_CurrLevel());

	if (pLevel == nullptr)
	{
		Safe_Release(pGameInstance);
		return true;
	}


	auto& tUser = pkt.tuser(0);

	wstring szNickName = CAsUtils::ToWString(tUser.strnickname());


	CLobbyUser_Server* pUser = pLevel->Find_LobbyUser(szNickName);

	if (pUser == nullptr)
	{
		Safe_Release(pGameInstance);
		return true;
	}

	pUser->Set_Character(CAsUtils::ToWString(tUser.strcharacter()));
	if (tUser.bready())
		pUser->Set_Ready();


	Safe_Release(pGameInstance);
	return true;
}

bool Handel_S_NEARTARGET_Server(PacketSessionRef& session, Protocol::S_NEARTARGET& pkt)
{
	return false;
}
