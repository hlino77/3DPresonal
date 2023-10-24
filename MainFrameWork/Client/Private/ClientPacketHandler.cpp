#include "stdafx.h"
#include "ClientPacketHandler.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "Level_Loading.h"
#include "ServerSessionManager.h"
#include "ServerSession.h"
#include "Player_Naruto.h"
#include "AsUtils.h"
#include "Monster.h"
#include "ColliderSphere.h"
#include "Level_Lobby.h"
#include "LobbyUser.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID_Client(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handel_S_TIME_Client(PacketSessionRef& session, Protocol::S_TIME& pkt)
{
	CServerSessionManager::TIME tServerTime, tClientTime;
	CServerSessionManager::GetInstance()->Get_ClientTime();


	LARGE_INTEGER iSendTick, iRecvTick, iCpu;
	
	iSendTick.QuadPart = pkt.isendtick();

	tServerTime.iMinute = pkt.iserverminute();
	tServerTime.fSecond = pkt.fserversecond();

	QueryPerformanceCounter(&iRecvTick);
	QueryPerformanceFrequency(&iCpu);

	uint64 iDelayTick = iRecvTick.QuadPart - iSendTick.QuadPart;
	_float fDelayTime = _float(iDelayTick) / iCpu.QuadPart;

	fDelayTime /= 2.0f;

	tClientTime = tServerTime;
	tClientTime.fSecond += fDelayTime;

	CServerSessionManager::GetInstance()->Set_ClientTime(tClientTime);
	CServerSessionManager::GetInstance()->Set_NetworkDelay(fDelayTime);

	return true;
}


bool Handel_S_OPENLEVEL_Client(PacketSessionRef& session, Protocol::S_OPEN_LEVEL& pkt)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	while (true)
	{
		if ((_uint)LEVELID::LEVEL_LOADING != pGameInstance->Get_CurrLevelIndex())
		{
			if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(pGameInstance->Get_Device(), pGameInstance->Get_Context(), (LEVELID)pkt.ilevelid(), L"Loading"))))
			{
				Safe_Release(pGameInstance);
				return true;
			}

			CServerSessionManager::GetInstance()->Get_ServerSession()->Set_LevelState(LEVELSTATE::LOADING);
			Protocol::S_LEVEL_STATE pkt;
			pkt.set_ilevelstate((_uint)LEVELSTATE::LOADING);

			SendBufferRef pSendBuffer = CClientPacketHandler::MakeSendBuffer(pkt);
			CServerSessionManager::GetInstance()->Send(pSendBuffer);
			break;
		}
	}

	Safe_Release(pGameInstance);

	return true;
}

bool Handel_S_LEVELSTATE_Client(PacketSessionRef& session, Protocol::S_LEVEL_STATE& pkt)
{
	ServerSessionRef& pServerSession = CServerSessionManager::GetInstance()->Get_ServerSession();
	pServerSession->Set_LevelState((LEVELSTATE)pkt.ilevelstate());
	return true;
}

bool Handel_S_CREATEOBJECT_Client(PacketSessionRef& session, Protocol::S_CREATE_OBJCECT& pkt)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	switch ((OBJ_TYPE)pkt.iobjecttype())
	{
	case OBJ_TYPE::PLAYER:
	{
		CPlayer::MODELDESC Desc;
		Desc.strFileName = CAsUtils::ToWString(pkt.strname());
		Desc.bControl = pkt.bcontroll();
		Desc.iObjectID = pkt.iobjectid();
		Desc.iLayer = pkt.ilayer();

		wstring szProtoName = L"Prototype_GameObject_Player_" + Desc.strFileName;
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Add_GameObject(pkt.ilevel(), pkt.ilayer(), szProtoName, &Desc));
		if (nullptr == pPlayer)
		{
			Safe_Release(pGameInstance);
			return true;
		}
			
		if(Desc.bControl)
			CServerSessionManager::GetInstance()->Set_Player(pPlayer);
		
		pPlayer->Get_TransformCom()->Set_State(CTransform::STATE::STATE_POSITION, Vec3(pkt.vpos().data()));
		break;
	}
	case OBJ_TYPE::MONSTER:
	{
		CMonster::MODELDESC Desc;
		Desc.strFileName = CAsUtils::ToWString(pkt.strname());
		Desc.iObjectID = pkt.iobjectid();
		Desc.iLayer = pkt.ilayer();

		wstring szProtoName = L"Prototype_GameObject_Monster_" + Desc.strFileName;
		CMonster* pMonster = dynamic_cast<CMonster*>(pGameInstance->Add_GameObject(pkt.ilevel(), pkt.ilayer(), szProtoName, &Desc));
		if (nullptr == pMonster)
		{
			Safe_Release(pGameInstance);
			return true;
		}
		pMonster->Get_TransformCom()->Set_State(CTransform::STATE::STATE_POSITION, Vec3(pkt.vpos().data()));

		break;
	}
	case OBJ_TYPE::PROP:
		break;
	}

	Safe_Release(pGameInstance);
}

bool Handel_S_CHARACTERNAME_Client(PacketSessionRef& session, Protocol::S_CHARACTER_NAME& pkt)
{
	return true;
}

bool Handel_S_MATRIX_Client(PacketSessionRef& session, Protocol::S_MATRIX& pkt)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CGameObject* pObject = pGameInstance->Find_GameObejct(pkt.ilevel(), pkt.ilayer(), pkt.iobjectid());

	if (pObject == nullptr)
	{
		Safe_Release(pGameInstance);
		return true;
	}
		

	CTransform* pTransform = dynamic_cast<CTransform*>(pObject->Get_Component(L"Com_Transform"));

	Matrix matWorld;

	memcpy(&matWorld.m[0], &pkt.matrix()[0], sizeof(_float) * 16);

	pTransform->Set_WorldMatrix(matWorld);
	Safe_Release(pGameInstance);


	return true;
}

bool Handel_S_ANIMATION_Client(PacketSessionRef& session, Protocol::S_ANIMATION& pkt)
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

bool Handel_S_OBJECTINFO_Client(PacketSessionRef& session, Protocol::S_OBJECTINFO& pkt)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_uint iUserPlayerID = CServerSessionManager::GetInstance()->Get_Player()->Get_ObjectID();


	for (_uint i = 0; i < pkt.tobject_size(); ++i)
	{
		auto tObject = pkt.mutable_tobject(i);

		_uint iObjectID = tObject->iobjectid();
		if (iObjectID == iUserPlayerID)
			continue;

		CGameObject* pObject = pGameInstance->Find_GameObejct(tObject->ilevel(), tObject->ilayer(), iObjectID);
		if (pObject == nullptr)
			continue;

		pObject->Set_TargetPos(Vec3(tObject->mutable_vtargetpos()->mutable_data()));
		pObject->Set_TargetMatrix(Matrix(tObject->mutable_matworld()->mutable_data()));
	}


	Safe_Release(pGameInstance);

	return true;
}

bool Handel_S_STATE_Client(PacketSessionRef& session, Protocol::S_STATE& pkt)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	auto tObject = pkt.tobject();

	CGameObject* pObject = pGameInstance->Find_GameObejct(tObject.ilevel(), tObject.ilayer(), tObject.iobjectid());
	if (pObject == nullptr || pObject->Is_Control() == true)
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


	wstring strState = CAsUtils::ToWString(pkt.strstate());
	pObject->Set_NoneControlState(strState);

	Safe_Release(pGameInstance);

	return true;
}

bool Handel_S_COLLIDERSTATE_Client(PacketSessionRef& session, Protocol::S_COLLIDERSTATE& pkt)
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
	pCollider->Set_Attack(pkt.iattack());

	Safe_Release(pGameInstance);

	return true;
}

bool Handel_S_COLLISION_Client(PacketSessionRef& session, Protocol::S_COLLISION& pkt)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	CGameObject* pObject = pGameInstance->Find_GameObejct(pkt.ilevel(), pkt.ilayer(), pkt.iobjectid());
	if (pObject == nullptr)
	{
		Safe_Release(pGameInstance);
		return true;
	}

	CGameObject* pOtherObject = pGameInstance->Find_GameObejct(pkt.ilevel(), pkt.iotherlayer(), pkt.iotherid());
	if (pObject == nullptr)
	{
		Safe_Release(pGameInstance);
		return true;
	}


	CCollider* pOtherCollider = pOtherObject->Get_Colider(pkt.iothercollayer());

	if (pkt.benter())
		pObject->OnCollisionEnter(pkt.icollayer(), pOtherCollider);
	else
		pObject->OnCollisionExit(pkt.icollayer(), pOtherCollider);


	Safe_Release(pGameInstance);

	return true;
}

bool Handel_S_NICKNAME_Client(PacketSessionRef& session, Protocol::S_NICKNAME& pkt)
{


	return true;
}

bool Handel_S_USERINFO_Client(PacketSessionRef& session, Protocol::S_USERINFO& pkt)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_CurrLevelIndex() != LEVELID::LEVEL_LOBBY)
	{
		Safe_Release(pGameInstance);
		return true;
	}
	

	CLevel_Lobby* pLevel = dynamic_cast<CLevel_Lobby*>(pGameInstance->Get_CurrLevel());

	if (pLevel == nullptr)
	{
		Safe_Release(pGameInstance);
		return true;
	}


	for (_uint i = 0; i < pkt.tuser_size(); ++i)
	{
		auto& tUser = pkt.tuser(i);

		wstring szNickName = CAsUtils::ToWString(tUser.strnickname());
		
		CLobbyUser* pUser = pLevel->Find_LobbyUser(szNickName);

		if (pUser == nullptr)
		{
			pLevel->Add_LobbyUser(szNickName);
			pUser = pLevel->Find_LobbyUser(szNickName);
			pUser->Set_Character(CAsUtils::ToWString(tUser.strcharacter()));
			if (tUser.bready())
				pUser->Set_Ready();
		}

		if (pUser->Get_NickName() == CServerSessionManager::GetInstance()->Get_NickName())
			continue;
		else
		{
			pUser->Set_Character(CAsUtils::ToWString(tUser.strcharacter()));
			if (tUser.bready())
				pUser->Set_Ready();
		}
		
	}


	Safe_Release(pGameInstance);
	return true;
}

bool Handel_S_NEARTARGET_Client(PacketSessionRef& session, Protocol::S_NEARTARGET& pkt)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	CGameObject* pObject = pGameInstance->Find_GameObejct(pkt.ilevel(), pkt.ilayer(), pkt.iobjectid());

	if (pObject == nullptr)
	{
		Safe_Release(pGameInstance);
		return true;
	}


	CGameObject* pNearTarget = pGameInstance->Find_GameObejct(pkt.ilevel(), pkt.itargetobjectlayer(), pkt.itargetobjectid());
	if (pNearTarget == nullptr)
	{
		Safe_Release(pGameInstance);
		return true;
	}
	pObject->Set_NearTarget(pNearTarget);

	Safe_Release(pGameInstance);

	return true;
}
