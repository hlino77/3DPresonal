#include "stdafx.h"
#include "State_Sasuke_WireStart.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"
#include "PickingMgr.h"
#include "GameInstance.h"
#include "Camera_Player.h"
#include "WireTrail.h"

CState_Sasuke_WireStart::CState_Sasuke_WireStart(const wstring& strStateName, CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_WireStart::CState_Sasuke_WireStart(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_WireStart::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"WireJump_Ready", 1.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	m_iBoneIndex = m_pPlayer->Get_ModelCom()->Find_BoneIndex(L"RightHandThumb3_end");

	if (m_iBoneIndex == -1)
		return E_FAIL;



	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Sasuke_WireStart::Tick_State_Control;
	else
		m_TickFunc = &CState_Sasuke_WireStart::Tick_State_NoneControl;


	return S_OK;
}

void CState_Sasuke_WireStart::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);

	m_pPlayer->Set_Invincible(true);



	m_pPlayer->Set_Gravity(false);
	m_pPlayer->Get_RigidBody()->UseDrag(false);
	m_pPlayer->Get_RigidBody()->UseGravity(false);
	m_pPlayer->Get_RigidBody()->SetCompareGruond(false);
	m_pPlayer->Get_RigidBody()->ClearForce(ForceMode::VELOCITY_CHANGE);


	if (m_pPlayer->Is_Control())
	{
		Vec3 vDir = m_pPlayer->Get_Camera()->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
		Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		vDir.Normalize();
		m_pPlayer->Set_TargetPos(vPos + (vDir * 21.0f));
	}

	m_bPickStart = false;
	m_bPicked = false;
}

void CState_Sasuke_WireStart::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Sasuke_WireStart::Exit_State()
{
	m_pPlayer->Get_RigidBody()->UseDrag(true);
	m_pPlayer->Set_Gravity(true);
	m_pPlayer->Get_RigidBody()->SetCompareGruond(true);

	if (m_pPlayer->Get_WireTrail()->Is_Disappear())
		m_pPlayer->Get_WireTrail()->Set_Active(false);
}

void CState_Sasuke_WireStart::Tick_State_Control(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();
	_uint iFrame = pPlayerModel->Get_Anim_Frame(m_iAnimIndex);


	if (iFrame >= 8)
	{
		if (m_bPickStart == false)
		{
			m_WirePicking = std::async(&CState_Sasuke_WireStart::WirePicking, this);
			m_pPlayer->Get_WireTrail()->Appear();
			m_bPickStart = true;
		}
		else
		{
			Vec3 vTargetPos = m_pPlayer->Get_TargetPos();
			Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
			Vec3 vDir = vTargetPos - vPos;
			_float fLength = vDir.Length();

			if (fLength < 21.0f)
				m_pPlayer->Get_WireTrail()->Set_TargetPos(m_pPlayer->Get_TargetPos());
		}
	}
	else
	{
		Vec3 vTargetPos = m_pPlayer->Get_TargetPos();
		Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		Vec3 vDir = vTargetPos - vPos;

		m_pPlayer->Get_TransformCom()->SetUp_Lerp(vDir, 15.0f, fTimeDelta);
		m_pPlayer->Get_TransformCom()->LookAt_Lerp_ForLand(vDir, 15.0f, fTimeDelta);
	}


	//Prototype_GameObject_RasenganCircle


	if (iFrame >= 17 && m_bPicked == false)
	{
		if (m_WirePicking.get())
		{
			m_pPlayer->Set_State(L"WireDash_Start");
		}
		else
			m_pPlayer->Get_WireTrail()->Disappear();

		m_bPicked = true;
	}



	if (pPlayerModel->Is_AnimationEnd(m_iAnimIndex))
	{
		m_pPlayer->Set_State(L"Fall_Front");
	}
	
}

void CState_Sasuke_WireStart::Tick_State_NoneControl(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();
	_uint iFrame = pPlayerModel->Get_Anim_Frame(m_iAnimIndex);


	if (iFrame >= 8)
	{
		if (m_bPickStart == false)
		{
			m_pPlayer->Get_WireTrail()->Appear();
			m_bPickStart = true;
		}
		else
		{
			Vec3 vTargetPos = m_pPlayer->Get_TargetPos();
			Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
			Vec3 vDir = vTargetPos - vPos;
			_float fLength = vDir.Length();

			if (fLength < 21.0f)
				m_pPlayer->Get_WireTrail()->Set_TargetPos(m_pPlayer->Get_TargetPos());
		}
	}
	else
	{
		Vec3 vTargetPos = m_pPlayer->Get_TargetPos();
		Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		Vec3 vDir = vTargetPos - vPos;

		m_pPlayer->Get_TransformCom()->SetUp_Lerp(vDir, 15.0f, fTimeDelta);
		m_pPlayer->Get_TransformCom()->LookAt_Lerp_ForLand(vDir, 15.0f, fTimeDelta);
	}


	//Prototype_GameObject_RasenganCircle
	

	if (iFrame >= 19)
	{
		if(m_pPlayer->Get_WireTrail()->Is_Disappear() == false)
			m_pPlayer->Get_WireTrail()->Disappear();
	}

	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

_bool CState_Sasuke_WireStart::WirePicking()
{
	_bool bPick = false;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	list<CGameObject*> m_ColMeshList = pGameInstance->Find_GameObjects(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_COLMESH);
	
	Matrix matWireWorld = m_pPlayer->Get_ModelCom()->Get_CurrBoneMatrix(m_iBoneIndex) * m_pPlayer->Get_TransformCom()->Get_WorldMatrix();
	Vec3 vWirePos(matWireWorld.m[3]);
	Vec3 vLook = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	vLook.Normalize();

	TRIAGLEDESC tResultDesc;
	tResultDesc.fDist = -1.0f;


	for (auto& ColMesh : m_ColMeshList)
	{
		TRIAGLEDESC tCurrDesc;
		tCurrDesc.fDist = -1.0f;

		if (CPickingMgr::GetInstance()->IsPicking(vWirePos, vLook, ColMesh, &tCurrDesc, 20.0f))
		{
			if (tResultDesc.fDist == -1.0f)
			{
				tResultDesc = tCurrDesc;
				bPick = true;
			}
			else
			{
				if (tCurrDesc.fDist < tResultDesc.fDist)
				{
					tResultDesc = tCurrDesc;
				}
			}
		}
	}

	if (bPick)
	{
		Vec3 vDir1 = tResultDesc.vPos1 - tResultDesc.vPos0;
		Vec3 vDir2 = tResultDesc.vPos2 - tResultDesc.vPos1;

		tResultDesc.vNormal = vDir1.Cross(vDir2);
		tResultDesc.vNormal.Normalize();

		Vec3 vTargetPos = vWirePos + vLook * tResultDesc.fDist;
		m_pPlayer->Set_TargetPos(vTargetPos);
		tResultDesc.fDist = -1.0f;
		m_pPlayer->Set_Wall(true);
		m_pPlayer->Set_Pick(tResultDesc);

	}
	

	Safe_Release(pGameInstance);
	return bPick;
}


void CState_Sasuke_WireStart::Free()
{
	__super::Free();
}
