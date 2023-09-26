#include "stdafx.h"
#include "State_Sasuke_Attack_Normal.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"

CState_Sasuke_Attack_Normal::CState_Sasuke_Attack_Normal(const wstring& strStateName, class CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_Attack_Normal::CState_Sasuke_Attack_Normal(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_Attack_Normal::Initialize()
{
	m_iAttack_cmb01.iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Attack_cmb01", 1.3f);
	m_iAttack_cmb02.iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Attack_cmb03", 1.0f);
	m_iAttack_cmb03.iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Attack_cmb08", 1.0f);
	m_iAttack_cmb04.iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Attack_cmb06", 1.3f);
	


	if (m_iAttack_cmb01.iAnimIndex == -1 ||
		m_iAttack_cmb02.iAnimIndex == -1 ||
		m_iAttack_cmb03.iAnimIndex == -1 ||
		m_iAttack_cmb04.iAnimIndex == -1)
		return E_FAIL;

	m_iAttack_cmb01.iChangeFrame = 14;
	m_iAttack_cmb02.iChangeFrame = 14;
	m_iAttack_cmb03.iChangeFrame = 16;
	m_iAttack_cmb04.iChangeFrame = 0;


	m_iAttack_cmb01.iInputNext = m_iAttack_cmb01.iChangeFrame - 5;
	m_iAttack_cmb02.iInputNext = m_iAttack_cmb02.iChangeFrame - 5;
	m_iAttack_cmb03.iInputNext = m_iAttack_cmb03.iChangeFrame - 5;
	m_iAttack_cmb04.iInputNext = 1000;

	m_iAttack_cmb01.iNextFrame = 0;
	m_iAttack_cmb02.iNextFrame = 1;
	m_iAttack_cmb03.iNextFrame = 4;
	m_iAttack_cmb04.iNextFrame = 0;


	m_Attack.push_back(m_iAttack_cmb01);
	m_Attack.push_back(m_iAttack_cmb02);
	m_Attack.push_back(m_iAttack_cmb03);
	m_Attack.push_back(m_iAttack_cmb04);

	return S_OK;
}

void CState_Sasuke_Attack_Normal::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAttack_cmb01.iAnimIndex, 0.1f, 0, 0);
	m_iCurrAttack = 0;
}

void CState_Sasuke_Attack_Normal::Tick_State(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();
	if (pPlayerModel->Get_CurrAnim() != m_Attack[m_iCurrAttack].iAnimIndex)
		return;

	if (pPlayerModel->Get_Anim_Frame(m_Attack[m_iCurrAttack].iAnimIndex) >= m_Attack[m_iCurrAttack].iInputNext)
	{
		if (KEY_TAP(KEY::LBTN))
		{
			if (m_iCurrAttack < m_Attack.size() - 1)
			{
				m_pPlayer->Reserve_Animation(m_Attack[m_iCurrAttack + 1].iAnimIndex, 0.1f, m_Attack[m_iCurrAttack].iNextFrame, m_Attack[m_iCurrAttack].iChangeFrame);
				++m_iCurrAttack;
			}
		}
	
	}

	if (pPlayerModel->Is_AnimationEnd(m_Attack[m_iCurrAttack].iAnimIndex))
		m_pPlayer->Set_State(L"Idle");

}

void CState_Sasuke_Attack_Normal::Exit_State()
{
}

void CState_Sasuke_Attack_Normal::Free()
{
	__super::Free();
}
