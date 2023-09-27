#include "stdafx.h"
#include "GameInstance.h"
#include "Player_Naruto.h"
#include "Key_Manager.h"
#include "Camera_Player.h"
#include "State_Naruto_Idle.h"
#include "State_Naruto_Move.h"
#include "State_Naruto_Attack_Normal.h"
#include "ServerSessionManager.h"

CPlayer_Naruto::CPlayer_Naruto(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPlayer(pDevice, pContext)
{
}

CPlayer_Naruto::CPlayer_Naruto(const CPlayer_Naruto& rhs)
	: CPlayer(rhs)
{
}

HRESULT CPlayer_Naruto::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Naruto::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	if (m_bControl)
		Ready_State();
	else
		Ready_AnimationSpeed();


	return S_OK;
}

void CPlayer_Naruto::Tick(_float fTimeDelta)
{
	m_pStateMachine->Tick_State(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CPlayer_Naruto::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);


	if (m_bControl)
	{
		m_fSendInfoTime += fTimeDelta;
		if (m_fSendInfoTime >= 0.1f)
		{
			m_fSendInfoTime = 0.0f;
			Send_PlayerInfo();
		}
	}
}

HRESULT CPlayer_Naruto::Render()
{
	__super::Render();
	return S_OK;
}

void CPlayer_Naruto::Send_PlayerInfo()
{
	Protocol::S_PLAYERINFO pkt;

	auto tPlayerInfo = pkt.add_tplayer();

	tPlayerInfo->set_iplayerid(m_iObjectID);
	tPlayerInfo->set_ilevel(CGameInstance::GetInstance()->Get_CurrLevelIndex());
	tPlayerInfo->set_ilayer((_uint)LAYER_TYPE::LAYER_PLAYER);


	auto vTargetPos = tPlayerInfo->mutable_vtargetpos();
	vTargetPos->Resize(3, 0.0f);
	memcpy(vTargetPos->mutable_data(), &m_vTargetPos, sizeof(Vec3));


	auto matWorld = tPlayerInfo->mutable_matworld();
	matWorld->Resize(16, 0.0f);
	Matrix matPlayerWorld = m_pTransformCom->Get_WorldMatrix();
	memcpy(matWorld->mutable_data(), &matPlayerWorld, sizeof(Matrix));


	SendBufferRef pSendBuffer = CClientPacketHandler::MakeSendBuffer(pkt);
	CServerSessionManager::GetInstance()->Send(pSendBuffer);
}


HRESULT CPlayer_Naruto::Ready_Components()
{
	__super::Ready_Components();
	return S_OK;
}

HRESULT CPlayer_Naruto::Ready_State()
{
	m_pStateMachine->Add_State(L"Idle", new CState_Naruto_Idle(L"Idle", this));
	m_pStateMachine->Add_State(L"Move", new CState_Naruto_Move(L"Move", this));
	m_pStateMachine->Add_State(L"Attack_Normal", new CState_Naruto_Attack_Normal(L"Attack_Normal", this));

	m_pStateMachine->Change_State(L"Idle");

	return S_OK;
}

HRESULT CPlayer_Naruto::Ready_AnimationSpeed()
{
	CState_Naruto_Idle(L"Idle", this).Initialize();
	CState_Naruto_Move(L"Move", this).Initialize();
	CState_Naruto_Attack_Normal(L"Attack_Normal", this).Initialize();

	return S_OK;
}

CPlayer_Naruto* CPlayer_Naruto::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Naruto* pInstance = new CPlayer_Naruto(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPlayer_Naruto");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Naruto::Clone(void* pArg)
{
	CPlayer_Naruto* pInstance = new CPlayer_Naruto(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayer_Naruto");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Naruto::Free()
{
	__super::Free();
}
