#include "stdafx.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Monster_WhiteZetsu.h"
#include "Key_Manager.h"
#include "Camera_Player.h"
#include "ServerSessionManager.h"
#include "Engine_Defines.h"
#include "ColliderSphere.h"
#include "State_WhiteZetsu_Idle.h"
#include "State_WhiteZetsu_HitMiddle.h"
#include "State_WhiteZetsu_ChasePlayer.h"
#include "State_WhiteZetsu_Appear.h"
#include "State_WhiteZetsu_Attack_Normal.h"
#include "State_WhiteZetsu_Attack_Kick.h"
#include "State_WhiteZetsu_Attack_Punch.h"
#include "State_WhiteZetsu_Dying_Normal.h"
#include "State_WhiteZetsu_HitSpinBlowUp.h"
#include "State_WhiteZetsu_GetUp.h"
#include "State_WhiteZetsu_FallBehind.h"
#include "State_WhiteZetsu_DownToFloor.h"
#include "State_WhiteZetsu_HitSpinBlowDown.h"
#include "State_WhiteZetsu_Die.h"

CMonster_WhiteZetsu::CMonster_WhiteZetsu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

CMonster_WhiteZetsu::CMonster_WhiteZetsu(const CMonster_WhiteZetsu& rhs)
	: CMonster(rhs)
{
}

HRESULT CMonster_WhiteZetsu::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_WhiteZetsu::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	Ready_State();


	m_tCullingSphere.Radius = 2.0f;

	return S_OK;
}

void CMonster_WhiteZetsu::Tick(_float fTimeDelta)
{
	m_pStateMachine->Tick_State(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CMonster_WhiteZetsu::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	Set_Colliders();
}

HRESULT CMonster_WhiteZetsu::Render()
{
	__super::Render();

	if (m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->IsActive())
		m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->DebugRender();

	if (m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->IsActive())
		m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->DebugRender();

	return S_OK;
}

void CMonster_WhiteZetsu::Set_Colliders()
{
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Center();

	if (m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->IsActive())
		m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Center();

}



HRESULT CMonster_WhiteZetsu::Ready_Components()
{
	__super::Ready_Components();
	return S_OK;
}

HRESULT CMonster_WhiteZetsu::Ready_State()
{
	m_pStateMachine->Add_State(L"Idle", new CState_WhiteZetsu_Idle(L"Idle", this));
	m_pStateMachine->Add_State(L"Hit_Middle", new CState_WhiteZetsu_HitMiddle(L"Hit_Middle", this));
	m_pStateMachine->Add_State(L"ChaseTarget", new CState_WhiteZetsu_ChasePlayer(L"ChaseTarget", this));
	m_pStateMachine->Add_State(L"Appear", new CState_WhiteZetsu_Appear(L"Appear", this));
	m_pStateMachine->Add_State(L"Attack_Normal", new CState_WhiteZetsu_Attack_Normal(L"Attack_Normal", this));
	m_pStateMachine->Add_State(L"Attack_Kick", new CState_WhiteZetsu_Attack_Kick(L"Attack_Kick", this));
	m_pStateMachine->Add_State(L"Attack_Punch", new CState_WhiteZetsu_Attack_Punch(L"Attack_Punch", this));
	m_pStateMachine->Add_State(L"Dying_Normal", new CState_WhiteZetsu_Dying_Normal(L"Dying_Normal", this));
	m_pStateMachine->Add_State(L"Hit_SpinBlowUp", new CState_WhiteZetsu_HitSpinBlowUp(L"Hit_SpinBlowUp", this));
	m_pStateMachine->Add_State(L"GetUp", new CState_WhiteZetsu_GetUp(L"GetUp", this));
	m_pStateMachine->Add_State(L"Fall_Behind", new CState_WhiteZetsu_FallBehind(L"Fall_Behind", this));
	m_pStateMachine->Add_State(L"DownToFloor", new CState_WhiteZetsu_DownToFloor(L"DownToFloor", this));
	m_pStateMachine->Add_State(L"Hit_SpinBlowDown", new CState_WhiteZetsu_HitSpinBlowDown(L"Hit_SpinBlowDown", this));
	m_pStateMachine->Add_State(L"Die", new CState_WhiteZetsu_Die(L"Die", this));



	m_pStateMachine->Change_State(L"Appear");

	return S_OK;
}


CMonster_WhiteZetsu* CMonster_WhiteZetsu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_WhiteZetsu* pInstance = new CMonster_WhiteZetsu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CMonster_WhiteZetsu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_WhiteZetsu::Clone(void* pArg)
{
	CMonster_WhiteZetsu* pInstance = new CMonster_WhiteZetsu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CMonster_WhiteZetsu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_WhiteZetsu::Free()
{
	__super::Free();
}
