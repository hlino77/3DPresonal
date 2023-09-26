#include "stdafx.h"
#include "ClientPacketHandler.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "Level_Loading.h"
#include "ServerSessionManager.h"
#include "ServerSession.h"
#include "Player_Naruto.h"
#include "AsUtils.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID_Client(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_S_TEST_Client(PacketSessionRef& session, Protocol::S_TEST& pkt)
{
	// TODO
	cout << pkt.id() << endl;
	cout << pkt.str() << endl;

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
				return E_FAIL;

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

		wstring szProtoName = L"Prototype_GameObject_Player_" + Desc.strFileName;
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Add_GameObject(pkt.ilevel(), pkt.ilayer(), szProtoName, &Desc));
		if (nullptr == pPlayer)
			return E_FAIL;
		if(Desc.bControl)
			CServerSessionManager::GetInstance()->Set_Player(pPlayer);
		break;
	}
	case OBJ_TYPE::MONSTER:
		break;
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
