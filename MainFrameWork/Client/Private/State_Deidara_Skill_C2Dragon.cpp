#include "stdafx.h"
#include "State_Deidara_Skill_C2Dragon.h"
#include "Boss_Deidara.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"


CState_Deidara_Skill_C2Dragon::CState_Deidara_Skill_C2Dragon(const wstring& strStateName, class CBoss_Deidara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Deidara_Skill_C2Dragon::CState_Deidara_Skill_C2Dragon(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Deidara_Skill_C2Dragon::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_C2Dragon", 1.0f);
	m_pBoss->Get_ModelCom()->Set_CurrAnim(m_iAnimIndex);
	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Deidara_Skill_C2Dragon::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_bDragon = false;
	m_bSound = false;
}

void CState_Deidara_Skill_C2Dragon::Tick_State(_float fTimeDelta)
{
	_uint iFrame = m_pBoss->Get_ModelCom()->Get_Anim_Frame(m_iAnimIndex);

	if (m_bSound == false && iFrame > 6)
	{
		CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"NinjutsuStart.wav", g_fVolume * 0.2f, m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), 40.0f);
		m_bSound = true;
	}


	if (m_bDragon == false && iFrame > 15)
	{
		m_pBoss->Spawn_C2Dragon();
		m_bDragon = true;
	}

	m_pBoss->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Deidara_Skill_C2Dragon::Exit_State()
{

}


void CState_Deidara_Skill_C2Dragon::Free()
{
	__super::Free();
}
