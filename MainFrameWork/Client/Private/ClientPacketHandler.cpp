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
			if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(pGameInstance->Get_Device(), pGameInstance->Get_Context(), (LEVELID)pkt.ilevelid()))))
			{
				Safe_Release(pGameInstance);
				return true;
			}
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

bool Handel_S_PLAYERINFO_Client(PacketSessionRef& session, Protocol::S_PLAYERINFO& pkt)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_uint iUserPlayerID = CServerSessionManager::GetInstance()->Get_Player()->Get_ObjectID();


	for (_uint i = 0; i < pkt.tplayer_size(); ++i)
	{
		auto tPlayer = pkt.mutable_tplayer(i);

		_uint iPlayerID = tPlayer->iplayerid();
		if (iPlayerID == iUserPlayerID)
			continue;

		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObejct(tPlayer->ilevel(), tPlayer->ilayer(), iPlayerID));
		if (pPlayer == nullptr)
			continue;

		pPlayer->Set_TargetPos(Vec3(tPlayer->mutable_vtargetpos()->mutable_data()));
		pPlayer->Set_TargetMatrix(Matrix(tPlayer->mutable_matworld()->mutable_data()));
	}


	Safe_Release(pGameInstance);

	return true;
}

bool Handel_S_STATE_Client(PacketSessionRef& session, Protocol::S_STATE& pkt)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	CGameObject* pObject = pGameInstance->Find_GameObejct(pkt.tplayer().ilevel(), pkt.tplayer().ilayer(), pkt.tplayer().iplayerid());
	if (pObject == nullptr || pObject->Is_Control() == true)
	{
		Safe_Release(pGameInstance);
		return true;
	}
		

	Vec3 vTargetPos(pkt.tplayer().vtargetpos().data());
	Matrix matTargetWorld(pkt.tplayer().matworld().data());


	pObject->Set_TargetPos(vTargetPos);
	pObject->Set_TargetMatrix(matTargetWorld);

	if (pkt.itargetobjectid() == -1)
		pObject->Reset_NearTarget();
	else
	{
		CGameObject* pNearTarget = pGameInstance->Find_GameObejct(pkt.tplayer().ilevel(), pkt.itargetobjectlayer(), pkt.itargetobjectid());
		if (pNearTarget == nullptr)
		{
			Safe_Release(pGameInstance);
			return true;
		}
		pObject->Set_NearTarget(pNearTarget);
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
