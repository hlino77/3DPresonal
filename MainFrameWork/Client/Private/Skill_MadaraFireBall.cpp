#include "stdafx.h"
#include "GameInstance.h"
#include "Skill_MadaraFireBall.h"
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


CSkill_MadaraFireBall::CSkill_MadaraFireBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill(pDevice, pContext)
{
}

CSkill_MadaraFireBall::CSkill_MadaraFireBall(const CSkill_MadaraFireBall& rhs)
	: CSkill(rhs)
{
}

HRESULT CSkill_MadaraFireBall::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkill_MadaraFireBall::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	{
		CGameObject* pFireBall = pGameInstance->Add_GameObject(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Effect_FireBall");

		m_pFireBall = dynamic_cast<CFireBall*>(pFireBall);
		m_pFireBall->Set_Owner(this);
		
		Vec3 vScale = m_pFireBall->Get_TransformCom()->Get_Scale();
		vScale *= 3.0f;
		m_pFireBall->Get_TransformCom()->Set_Scale(vScale);

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

		Vec3 vScale = m_pPlane->Get_TransformCom()->Get_Scale();
		vScale *= 3.0f;
		m_pPlane->Get_TransformCom()->Set_Scale(vScale);

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

		Vec3 vScale = m_pTrail->Get_TransformCom()->Get_Scale();
		vScale *= 3.0f;
		m_pTrail->Get_TransformCom()->Set_Scale(vScale);



		if (m_pTrail == nullptr)
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}
	}

	
	Safe_Release(pGameInstance);

	Set_Active(false);


	m_iAttack = 10;

	m_fEffectTime = 0.1f;

    return S_OK;
}

void CSkill_MadaraFireBall::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	
	if (m_bExplosion)
	{
		Set_Active(false);
	}
	else
	{
		Effect_Shooting(fTimeDelta);

		Follow_Target(fTimeDelta);
	}

}

void CSkill_MadaraFireBall::LateTick(_float fTimeDelta)
{
}

HRESULT CSkill_MadaraFireBall::Render()
{
	return S_OK;
}

void CSkill_MadaraFireBall::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{

}

void CSkill_MadaraFireBall::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
	
}

void CSkill_MadaraFireBall::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
	m_pSkillOwner->Set_SlowMotion(false);
}

HRESULT CSkill_MadaraFireBall::Ready_Coliders()
{
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Radius(4.0f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Offset(Vec3(0.0f, 0.0f, 0.0f));
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_BODY);

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Radius(5.0f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Offset(Vec3(0.0f, 0.0f, 0.0f));
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_AttackCollider(m_iAttack, (_uint)COLLIDER_ATTACK::SPINBLOWUP, false);
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);


	return S_OK;
}

void CSkill_MadaraFireBall::Explosion()
{
	m_bExplosion = true;

	m_pFireBall->DisAppear();
	m_pPlane->DisAppear();
	m_pTrail->DisAppear();

	Effect_Explosion();


	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"FireBallBomb.wav", g_fVolume * 0.4f, m_pTransformCom->Get_State(CTransform::STATE_POSITION), 40.0f);
}

void CSkill_MadaraFireBall::Appear()
{
	Set_Active(true);
	m_bExplosion = false;

	m_pNearTarget = m_pSkillOwner->Get_NearTarget();

	Vec3 vPlayerPos = m_pSkillOwner->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vPlayerLook = m_pSkillOwner->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	Vec3 vPlayerUp = m_pSkillOwner->Get_TransformCom()->Get_State(CTransform::STATE_UP);
	vPlayerLook.Normalize();
	vPlayerUp.Normalize();

	Vec3 vPos = vPlayerPos + (vPlayerLook * 4.0f) + (vPlayerUp * 3.5f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->LookAt(vPos + vPlayerLook);


	m_pFireBall->Appear();
	m_pPlane->Appear();
	m_pTrail->Appear();

	m_fCurrEffectTime = m_fEffectTime;

	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"FireBallStart.wav", g_fVolume * 0.4f, m_pTransformCom->Get_State(CTransform::STATE_POSITION), 40.0f);
}



HRESULT CSkill_MadaraFireBall::Ready_Components()
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

void CSkill_MadaraFireBall::Follow_Target(_float fTimeDelta)
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

void CSkill_MadaraFireBall::Effect_Explosion()
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	{
		_float fTime = 0.0f;
		_float fLength = 0.2f;

		for (_uint i = 0; i < 20; ++i)
		{
			for (_uint k = 0; k < 15; ++k)
			{
				CFire* pFire = CPool<CFire>::Get_Obj();

				pFire->Appear(vPos, fLength, fTime);
			}

			fTime += 0.02f;
			fLength += 0.3f;
		}
	}


	{
		for (_uint i = 0; i < 50; ++i)
		{
			CSmoke_24* pSmoke = CPool<CSmoke_24>::Get_Obj();

			pSmoke->Appear(vPos, Vec4(0.0f, 0.0f, 0.0f, 0.7f), Vec2(2.0f, 2.0f), 0.02f, 0.04f, 0.05f);
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
				pLineCircle->Appear(vPos, vColor, vBlurColor, 5.0f);
			}
		}
	}


	CExplosionRing* pExplosionRing = CPool<CExplosionRing>::Get_Obj();

	pExplosionRing->Appear(vPos, Vec3(40.0f, 40.0f, 0.0f));

	CServerSessionManager::GetInstance()->Get_Player()->Get_Camera()->Cam_Shake(0.002f, 0.2f);
}

void CSkill_MadaraFireBall::Effect_Shooting(_float fTimeDelta)
{
	m_fCurrEffectTime += fTimeDelta;
	if (m_fCurrEffectTime >= m_fEffectTime)
	{
		Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		{
			_float fTime = 0.1f;
			_float fLength = 4.0f;

			for (_uint k = 0; k < 25; ++k)
			{
				CFire* pFire = CPool<CFire>::Get_Obj();

				pFire->Appear(vPos, fLength, fTime);
			}
		}
		m_fCurrEffectTime = 0.0f;
	}
}



CSkill_MadaraFireBall* CSkill_MadaraFireBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_MadaraFireBall* pInstance = new CSkill_MadaraFireBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		cout << "Failed To Created : CSkill_MadaraFireBall" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_MadaraFireBall::Clone(void* pArg)
{
	CSkill_MadaraFireBall* pInstance = new CSkill_MadaraFireBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CSkill_MadaraFireBall" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_MadaraFireBall::Free()
{
	__super::Free();
}
