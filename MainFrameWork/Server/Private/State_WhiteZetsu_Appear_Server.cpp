#include "stdafx.h"
#include "State_WhiteZetsu_Appear_Server.h"
#include "Monster_WhiteZetsu_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_WhiteZetsu_Appear_Server::CState_WhiteZetsu_Appear_Server(const wstring& strStateName, class CMonster_WhiteZetsu_Server* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_Appear_Server::CState_WhiteZetsu_Appear_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_Appear_Server::Initialize()
{
	m_iAppear = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Appearance_Type01", 1.0f);
	m_pMonster->Get_ModelCom()->Set_CurrAnim(m_iAppear);
	if (m_iAppear == -1)
		return E_FAIL;
	
	return S_OK;
}

void CState_WhiteZetsu_Appear_Server::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAppear, 0.1f, 0, 0);

	m_bLookAt = false;
}

void CState_WhiteZetsu_Appear_Server::Tick_State(_float fTimeDelta)
{
	/*if (m_pMonster->Get_ModelCom()->Is_AnimationEnd(m_iAppear))
		m_pMonster->Set_State(L"Idle");*/

	if (m_bLookAt == false)
	{
		Set_LookAt_Player();
		m_bLookAt = true;
	}


	if (m_pMonster->Get_ModelCom()->Get_Anim_Frame(m_iAppear) >= 30)
		m_pMonster->Set_State(L"Idle");
}

void CState_WhiteZetsu_Appear_Server::Exit_State()
{

}

void CState_WhiteZetsu_Appear_Server::Set_LookAt_Player()
{
	m_pMonster->Find_NearTarget();


	CGameObject* pPlayer = m_pMonster->Get_NearTarget();

	if (pPlayer)
	{
		Vec3 vPos = pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		m_pMonster->Get_TransformCom()->LookAt_ForLandObject(vPos);
	}
}


void CState_WhiteZetsu_Appear_Server::Free()
{
	__super::Free();
}
