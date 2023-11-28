#include "stdafx.h"
#include "GameInstance.h"
#include "Skill_FireBall.h"
#include "AsUtils.h"
#include "ColliderSphere.h"
#include "ServerSessionManager.h"
#include "RigidBody.h"
#include "RasenganCircle.h"
#include "Player_Naruto.h"
#include "Camera_Player.h"
#include "LineCircle.h"
#include "Pool.h"
#include "FireBall.h"
#include "FireBallPlane.h"
#include "FireBallTrail.h"
#include "Fire.h"
#include "Smoke_24.h"
#include "ExplosionRing.h"


CSkill_FireBall::CSkill_FireBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill(pDevice, pContext)
{
}

CSkill_FireBall::CSkill_FireBall(const CSkill_FireBall& rhs)
	: CSkill(rhs)
{
}

HRESULT CSkill_FireBall::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkill_FireBall::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	{
		CGameObject* pFireBall = pGameInstance->Add_GameObject(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Effect_FireBall");

		m_pFireBall = dynamic_cast<CFireBall*>(pFireBall);
		m_pFireBall->Set_Owner(this);

		if (m_pFireBall == nullptr)
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}
	}

	{
		CGameObject* pFirePlane = pGameInstance->Add_GameObject(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Effect_FireBallPlane");



		m_pPlane = dynamic_cast<CFireBallPlane*>(pFirePlane);
		m_pPlane->Set_Owner(this);

		if (m_pPlane == nullptr)
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}
	}
		
		
	{
		CGameObject* pFireBallTrail = pGameInstance->Add_GameObject(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Effect_FireBallTrail");

		m_pTrail = dynamic_cast<CFireBallTrail*>(pFireBallTrail);
		m_pTrail->Set_Owner(this);

		if (m_pTrail == nullptr)
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}
	}

	
	Safe_Release(pGameInstance);

	Set_Active(false);


	m_fExplosionTime = 0.1f;
	m_fSkillTime = 2.0f;

	m_iAttack = 20;

	m_fEffectTime = 0.1f;

    return S_OK;
}

void CSkill_FireBall::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	
	if (m_bExplosion)
	{
		m_fCurrTime -= fTimeDelta;
		if (m_fCurrTime <= 0.0f)
		{
			if (m_pSkillOwner->Is_Control())
			{
				m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(false);
				Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
			}
				

			Set_Active(false);
		}
	}
	else
	{
		m_fCurrTime += fTimeDelta;
		if (m_fCurrTime >= m_fSkillTime)
			Explosion();

		if (m_pSkillOwner->Is_Control())
		{
			m_fSendInfoTime += fTimeDelta;
			if (m_fSendInfoTime >= 0.05f)
			{
				m_fSendInfoTime = 0.0f;
				Send_SkillInfo();
			}
		}

		Effect_Shooting(fTimeDelta);

		Follow_Target(fTimeDelta);
	}
}

void CSkill_FireBall::LateTick(_float fTimeDelta)
{
}

HRESULT CSkill_FireBall::Render()
{
	return S_OK;
}

void CSkill_FireBall::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		if (m_bExplosion == false)
			Explosion();
	}

	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_ATTACK)
	{
		if (pOther->Get_Owner()->Is_Invincible())
			return;

		Add_Hit();
	}
}

void CSkill_FireBall::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
	
}

void CSkill_FireBall::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
	m_pSkillOwner->Set_SlowMotion(false);
}

HRESULT CSkill_FireBall::Ready_Coliders()
{
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Radius(3.0f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Offset(Vec3(0.0f, 0.0f, 0.0f));
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_BODY);

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Radius(4.0f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Offset(Vec3(0.0f, 0.0f, 0.0f));
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_AttackCollider(m_iAttack, (_uint)COLLIDER_ATTACK::SPINBLOWUP, false);
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);


	return S_OK;
}

void CSkill_FireBall::Send_SkillInfo()
{
	Protocol::S_OBJECTINFO pkt;

	auto tObject = pkt.add_tobject();

	tObject->set_iobjectid(m_iObjectID);
	tObject->set_ilevel(CGameInstance::GetInstance()->Get_CurrLevelIndex());
	tObject->set_ilayer((_uint)LAYER_TYPE::LAYER_SKILL);


	auto vTargetPos = tObject->mutable_vtargetpos();
	vTargetPos->Resize(3, 0.0f);
	Vec3 vPlayerTargetPos = m_vTargetPos.load();
	memcpy(vTargetPos->mutable_data(), &vPlayerTargetPos, sizeof(Vec3));


	auto matWorld = tObject->mutable_matworld();
	matWorld->Resize(16, 0.0f);
	Matrix matPlayerWorld = m_pTransformCom->Get_WorldMatrix();
	memcpy(matWorld->mutable_data(), &matPlayerWorld, sizeof(Matrix));


	SendBufferRef pSendBuffer = CClientPacketHandler::MakeSendBuffer(pkt);
	CServerSessionManager::GetInstance()->Send(pSendBuffer);
}


void CSkill_FireBall::Explosion()
{
	if(m_pSkillOwner->Is_Control())
		Send_Explosion();

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Center();


	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(false);
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_BODY);


	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(true);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_AttackCollider(m_iAttack, (_uint)COLLIDER_ATTACK::SPINBLOWUP, false);
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);

	m_bExplosion = true;
	m_fCurrTime = m_fExplosionTime;

	m_pFireBall->DisAppear();
	m_pPlane->DisAppear();
	m_pTrail->DisAppear();

	Effect_Explosion();
}

void CSkill_FireBall::Appear()
{
	Set_Active(true);
	m_bExplosion = false;
	m_fCurrTime = 0.0f;

	m_pNearTarget = m_pSkillOwner->Get_NearTarget();

	Vec3 vPlayerPos = m_pSkillOwner->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vPlayerLook = m_pSkillOwner->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	Vec3 vPlayerUp = m_pSkillOwner->Get_TransformCom()->Get_State(CTransform::STATE_UP);
	vPlayerLook.Normalize();
	vPlayerUp.Normalize();

	Vec3 vPos = vPlayerPos + (vPlayerLook * 2.0f) + (vPlayerUp * 1.5f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->LookAt(vPos + vPlayerLook);
	Send_SkillInfo();




	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(true);
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_BODY);


	m_pFireBall->Appear();
	m_pPlane->Appear();
	m_pTrail->Appear();

	m_fCurrEffectTime = m_fEffectTime;
}



HRESULT CSkill_FireBall::Ready_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_UseLock_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	{
		CCollider::ColliderInfo tColliderInfo;
		tColliderInfo.m_bActive = true;
		tColliderInfo.m_iLayer = (_uint)LAYER_COLLIDER::LAYER_BODY;
		CSphereCollider* pCollider = nullptr;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_SphereColider"), TEXT("Com_SphereColider"), (CComponent**)&pCollider, &tColliderInfo)))
			return E_FAIL;

		m_Coliders.emplace((_uint)LAYER_COLLIDER::LAYER_BODY, pCollider);
	}

	{
		CCollider::ColliderInfo tColliderInfo;
		tColliderInfo.m_bActive = false;
		tColliderInfo.m_iLayer = (_uint)LAYER_COLLIDER::LAYER_ATTACK;
		CSphereCollider* pCollider = nullptr;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_SphereColider"), TEXT("Com_ColliderAttack"), (CComponent**)&pCollider, &tColliderInfo)))
			return E_FAIL;
		if (pCollider)
			m_Coliders.emplace((_uint)LAYER_COLLIDER::LAYER_ATTACK, pCollider);
	}


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;



	Safe_Release(pGameInstance);

	return S_OK;
}

void CSkill_FireBall::Add_Hit()
{
	if (m_pSkillOwner->Is_Control())
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pSkillOwner);

		pPlayer->Add_Hit();
	}	
}

void CSkill_FireBall::Follow_Target(_float fTimeDelta)
{
	if (m_pNearTarget)
	{
		Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		Vec3 vTargetPos = m_pNearTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		vTargetPos.y += 1.5f;

		Vec3 vDir = vTargetPos - vPos;
		vDir.Normalize();

		m_pTransformCom->SetUp_Lerp(vDir, 0.5f, fTimeDelta);
		m_pTransformCom->LookAt_Lerp_ForLand(vDir, 0.5f, fTimeDelta);
	}

	m_pTransformCom->Go_Straight(10.0f, fTimeDelta);
}

void CSkill_FireBall::Effect_Explosion()
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	{
		_float fTime = 0.0f;
		_float fLength = 0.2f;

		for (_uint i = 0; i < 15; ++i)
		{
			for (_uint k = 0; k < 15; ++k)
			{
				CFire* pFire = CPool<CFire>::Get_Obj();

				pFire->Appear(vPos, fLength, fTime);
			}

			fTime += 0.02f;
			fLength += 0.2f;
		}
	}


	{
		for (_uint i = 0; i < 30; ++i)
		{
			CSmoke_24* pSmoke = CPool<CSmoke_24>::Get_Obj();

			pSmoke->Appear(vPos, Vec4(0.0f, 0.0f, 0.0f, 0.7f), Vec2(1.0f, 1.0f), 0.01f, 0.01f, 0.05f);
		}
	}

	{
		Vec4 vColor(1.0f, 1.0f, 0.0f, 1.0f);
		Vec4 vBlurColor(1.0f, 0.3f, 0.0f, 1.0f);

		for (_uint i = 0; i < 200; ++i)
		{
			CLineCircle* pLineCircle = CPool<CLineCircle>::Get_Obj();
			if (pLineCircle)
			{
				pLineCircle->Appear(vPos, vColor, vBlurColor, 3.0f);
			}
		}
	}


	CExplosionRing* pExplosionRing = CPool<CExplosionRing>::Get_Obj();

	pExplosionRing->Appear(vPos, Vec3(20.0f, 20.0f, 0.0f));

	if (m_pSkillOwner->Is_Control())
		dynamic_cast<CPlayer*>(m_pSkillOwner)->Get_Camera()->Cam_Shake(0.001f, 0.2f);
}

void CSkill_FireBall::Effect_Shooting(_float fTimeDelta)
{
	m_fCurrEffectTime += fTimeDelta;
	if (m_fCurrEffectTime >= m_fEffectTime)
	{
		Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		{
			_float fTime = 0.1f;
			_float fLength = 2.0f;

			for (_uint k = 0; k < 15; ++k)
			{
				CFire* pFire = CPool<CFire>::Get_Obj();

				pFire->Appear(vPos, fLength, fTime);
			}
		}
		m_fCurrEffectTime = 0.0f;
	}
}



CSkill_FireBall* CSkill_FireBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_FireBall* pInstance = new CSkill_FireBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		cout << "Failed To Created : CSkill_FireBall" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_FireBall::Clone(void* pArg)
{
	CSkill_FireBall* pInstance = new CSkill_FireBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CSkill_FireBall" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_FireBall::Free()
{
	__super::Free();
}
