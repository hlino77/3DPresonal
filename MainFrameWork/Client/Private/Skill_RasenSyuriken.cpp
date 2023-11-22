#include "stdafx.h"
#include "GameInstance.h"
#include "Skill_RasenSyuriken.h"
#include "AsUtils.h"
#include "ColliderSphere.h"
#include "ServerSessionManager.h"
#include "RigidBody.h"
#include "Player_Naruto.h"
#include "Camera_Player.h"
#include "LineCircle.h"
#include "Pool.h"
#include "RasenSyurikenBladeBlur.h"
#include "RasenSyurikenBlade.h"
#include "RasenSyurikenBladeLine.h"
#include "RasenSyurikenCenter.h"
#include "SmokeRing.h"
#include "RasenSyurikenSphere.h"



CSkill_RasenSyuriken::CSkill_RasenSyuriken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill(pDevice, pContext)
{
}

CSkill_RasenSyuriken::CSkill_RasenSyuriken(const CSkill_RasenSyuriken& rhs)
	: CSkill(rhs)
{
}

HRESULT CSkill_RasenSyuriken::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkill_RasenSyuriken::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	{
		CGameObject* pCenter = pGameInstance->Add_GameObject(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Effect_RasenSyurikenCenter");

		m_pCenter = dynamic_cast<CRasenSyurikenCenter*>(pCenter);
		m_pCenter->Set_Owner(this);

		if (m_pCenter == nullptr)
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}
	}


	{
		CGameObject* pBladeBlur = pGameInstance->Add_GameObject(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Effect_RasenSyurikenBladeBlur");

		m_pBlur = dynamic_cast<CRasenSyurikenBladeBlur*>(pBladeBlur);
		m_pBlur->Set_Owner(this);

		if (m_pBlur == nullptr)
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}
	}

	{
		CGameObject* pLine = pGameInstance->Add_GameObject(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Effect_RasenSyurikenBladeLine");

		m_pLine = dynamic_cast<CRasenSyurikenBladeLine*>(pLine);
		m_pLine->Set_Owner(this);

		if (m_pLine == nullptr)
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}
	}


	{
		CGameObject* pBlade = pGameInstance->Add_GameObject(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Effect_RasenSyurikenBlade");

		m_pBlade = dynamic_cast<CRasenSyurikenBlade*>(pBlade);
		m_pBlade->Set_Owner(this);

		if (m_pBlade == nullptr)
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}
	}

	
	Safe_Release(pGameInstance);

	Set_Active(false);


	m_fExplosionTime = 0.5f;
	m_fSkillTime = 2.0f;

	m_iAttack = 10;

	m_fEffectTime = 0.1f;

	m_iBoneIndex = m_pSkillOwner->Get_ModelCom()->Find_BoneIndex(L"R_Hand_Weapon_cnt_tr_end");


    return S_OK;
}

void CSkill_RasenSyuriken::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bShoot == false)
	{
		Update_Transform();
		return;
	}

	
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
	}

	Follow_Target(fTimeDelta);
}

void CSkill_RasenSyuriken::LateTick(_float fTimeDelta)
{
}

HRESULT CSkill_RasenSyuriken::Render()
{
	return S_OK;
}

void CSkill_RasenSyuriken::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		if (m_bExplosion == false)
			Explosion();
	}
}

void CSkill_RasenSyuriken::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
	
}

void CSkill_RasenSyuriken::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
	m_pSkillOwner->Set_SlowMotion(false);
}

HRESULT CSkill_RasenSyuriken::Ready_Coliders()
{
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Radius(3.0f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Offset(Vec3(0.0f, 0.0f, 0.0f));
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_BODY);

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Radius(5.0f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Offset(Vec3(0.0f, 0.0f, 0.0f));
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_AttackCollider(m_iAttack, (_uint)COLLIDER_ATTACK::SPINBLOWUP, false);
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);


	return S_OK;
}

void CSkill_RasenSyuriken::Send_SkillInfo()
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


void CSkill_RasenSyuriken::Explosion()
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


	m_pCenter->DisAppear();
	m_pBlade->DisAppear();
	m_pBlur->DisAppear();
	m_pLine->DisAppear();
	

	Effect_Explosion();
}

void CSkill_RasenSyuriken::Appear()
{
	Update_Transform();
	Set_Active(true);
	m_bExplosion = false;
	m_fCurrTime = 0.0f;
	m_bShoot = false;

	m_fCurrEffectTime = m_fEffectTime;

	m_pCenter->Appear();
	m_pBlade->Appear();
	m_pBlur->Appear();
	m_pLine->Appear();

	Effect_SmokeRing(0.0f, 0.1f);
	Effect_SmokeRing(0.0f, 0.1f);
	Effect_SmokeRing(0.0f, 0.1f);
	Effect_SmokeRing(0.0f, 0.1f);
}

void CSkill_RasenSyuriken::Shoot()
{
	m_pNearTarget = m_pSkillOwner->Get_NearTarget();

	Vec3 vPlayerPos = m_pSkillOwner->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vPlayerLook = m_pSkillOwner->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	Vec3 vPlayerUp = m_pSkillOwner->Get_TransformCom()->Get_State(CTransform::STATE_UP);
	vPlayerLook.Normalize();
	vPlayerUp.Normalize();

	Vec3 vPos = vPlayerPos + (vPlayerLook * 2.0f) + (vPlayerUp * 1.5f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->LookAt(vPos + vPlayerLook);


	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(true);
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_BODY);


	m_bShoot = true;
}



HRESULT CSkill_RasenSyuriken::Ready_Components()
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

void CSkill_RasenSyuriken::Effect_SmokeRing(_float fTime, _float fBaseScale)
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	for (_uint i = 0; i < 8; ++i)
	{
		CSmokeRing* pSmokeRing = CPool<CSmokeRing>::Get_Obj();
		pSmokeRing->Appear(vPos, fBaseScale, Vec4(0.5f, 0.84f, 1.0f, 1.0f), Vec4(0.5f, 0.84f, 1.0f, 1.0f), fTime);
	}
}

void CSkill_RasenSyuriken::Add_Hit()
{
	
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pSkillOwner);

	pPlayer->Add_Hit();
}

void CSkill_RasenSyuriken::Follow_Target(_float fTimeDelta)
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

	m_pTransformCom->Go_Straight(8.0f, fTimeDelta);
}

void CSkill_RasenSyuriken::Effect_Explosion()
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


	{
		CPool<CRasenSyurikenSphere>::Get_Obj()->Appear(vPos);
	}


	{
		Vec4 vColor(0.03f, 0.69f, 0.94f, 1.0f);
		Vec4 vBlurColor(0.03f, 0.49f, 0.94f, 1.0f);

		for (_uint i = 0; i < 200; ++i)
		{
			CLineCircle* pLineCircle = CPool<CLineCircle>::Get_Obj();
			if (pLineCircle)
			{
				pLineCircle->Appear(vPos, vColor, vBlurColor, 4.0f);
			}
		}
	}

	if(m_pSkillOwner->Is_Control())
		dynamic_cast<CPlayer*>(m_pSkillOwner)->Get_Camera()->Cam_Shake(0.001f, 0.2f);
}

void CSkill_RasenSyuriken::Update_Transform()
{
	Matrix matPlayerWorld = m_pSkillOwner->Get_TransformCom()->Get_WorldMatrix();
	Matrix matBone = m_pSkillOwner->Get_ModelCom()->Get_CurrBoneMatrix(m_iBoneIndex);

	m_pTransformCom->Set_WorldMatrix(matBone * matPlayerWorld);

	Vec3 vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	Vec3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	Vec3 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	vUp.Normalize();
	vLook.Normalize();
	vRight.Normalize();

	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vLook);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, -vRight);
}



CSkill_RasenSyuriken* CSkill_RasenSyuriken::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_RasenSyuriken* pInstance = new CSkill_RasenSyuriken(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		cout << "Failed To Created : CSkill_RasenSyuriken" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_RasenSyuriken::Clone(void* pArg)
{
	CSkill_RasenSyuriken* pInstance = new CSkill_RasenSyuriken(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CSkill_RasenSyuriken" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_RasenSyuriken::Free()
{
	__super::Free();
}
