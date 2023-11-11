#include "stdafx.h"
#include "State_WhiteZetsu_Appear.h"
#include "Monster_WhiteZetsu.h"
#include "Client_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_WhiteZetsu_Appear::CState_WhiteZetsu_Appear(const wstring& strStateName, class CMonster_WhiteZetsu* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_Appear::CState_WhiteZetsu_Appear(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_Appear::Initialize()
{
	m_iAppear = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Appearance_Type01", 1.0f);
	m_pMonster->Get_ModelCom()->Set_CurrAnim(m_iAppear);
	if (m_iAppear == -1)
		return E_FAIL;

	return S_OK;
}

void CState_WhiteZetsu_Appear::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAppear, 0.1f, 0, 0);

	m_bLookAt = false;
}

void CState_WhiteZetsu_Appear::Tick_State(_float fTimeDelta)
{
	if (m_bLookAt == false)
	{
		Set_LookAt_Player();
		m_bLookAt = true;
	}
		

	m_pMonster->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_WhiteZetsu_Appear::Exit_State()
{

}

void CState_WhiteZetsu_Appear::Set_LookAt_Player()
{
	m_pMonster->Find_NearTarget();


	CGameObject* pPlayer = m_pMonster->Get_NearTarget();

	if (pPlayer)
	{
		Vec3 vPos = pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		m_pMonster->Get_TransformCom()->LookAt_ForLandObject(vPos);
	}
}


void CState_WhiteZetsu_Appear::Free()
{
	__super::Free();
}
