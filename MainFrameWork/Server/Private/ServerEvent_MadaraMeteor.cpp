#include "stdafx.h"
#include "ServerEvent_MadaraMeteor.h"
#include "EventMgr.h"
#include "Boss_Madara_Server.h"
#include "GameInstance.h"

CServerEvent_MadaraMeteor::CServerEvent_MadaraMeteor(_uint iID, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CServerEvent(iID, pDevice, pContext)
{
}

HRESULT CServerEvent_MadaraMeteor::Initialize()
{
	m_vMadaraPos = Vec3(80.41f, 27.91f, -33.61f);
	return S_OK;
}

void CServerEvent_MadaraMeteor::Enter_Event()
{
	Reset_ClientEventState();

	Send_State(EVENTSTATE::READY);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pMadara = dynamic_cast<CBoss_Madara_Server*>(pGameInstance->Find_GameObejct(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_BOSS, L"Madara"));
	m_iAnimIndex = m_pMadara->Get_ModelCom()->Find_AnimIndex(L"Ninjutsu_ShatteredHeaven");

	Safe_Release(pGameInstance);

	while (true)
	{
		if (Check_ClientState(EVENTSTATE::READY))
		{
			Send_State(EVENTSTATE::EVENT);
			break;
		}
			
	}

	
	m_pMadara->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, m_vMadaraPos);
	m_pMadara->Set_Invincible(true);

	m_pMadara->ResetSkill(CBoss_Madara_Server::MADARA_SKILL::FIREBALL);
	m_pMadara->Set_TeleportAttackDelay(100.0f);
	m_pMadara->Set_State(L"Idle");

	m_bSkill = false;
	m_fDelayTime = 1.0f;
}


void CServerEvent_MadaraMeteor::Exit_Event()
{

}

void CServerEvent_MadaraMeteor::Tick(_float fTimeDelta)
{
	m_pMadara->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, m_vMadaraPos);
	m_pMadara->Get_TransformCom()->LookAt(m_vMadaraPos + Vec3(0.0f, 0.0f, -1.0f));

	if (m_bSkill == false)
	{
		m_fDelayTime -= fTimeDelta;
		if (m_fDelayTime <= 0.0f)
		{
			m_pMadara->Set_State(L"Skill_Meteor");
			m_bSkill = true;
		}
	}
	else
	{
		if (m_pMadara->Get_ModelCom()->Get_Anim_Frame(m_iAnimIndex) > 75)
		{
			Send_State(EVENTSTATE::SETEND);
			CEventMgr::GetInstance()->End_Event(m_iEventID);
		}
	}	
}

void CServerEvent_MadaraMeteor::LateTick(_float fTimeDelta)
{

}

HRESULT CServerEvent_MadaraMeteor::Render()
{
	return S_OK;
}

void CServerEvent_MadaraMeteor::Free()
{
}
