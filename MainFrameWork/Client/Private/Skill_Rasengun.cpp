#include "stdafx.h"
#include "GameInstance.h"
#include "Skill_Rasengun.h"
#include "AsUtils.h"
#include "ColliderSphere.h"
#include "ServerSessionManager.h"
#include "RigidBody.h"
#include "RasenganCircle.h"
#include "Player_Naruto.h"
#include "Camera_Player.h"
#include "LineCircle.h"
#include "Pool.h"
#include "SmokeRing.h"

CSkill_Rasengun::CSkill_Rasengun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill(pDevice, pContext)
{
}

CSkill_Rasengun::CSkill_Rasengun(const CSkill_Rasengun& rhs)
	: CSkill(rhs)
{
}

HRESULT CSkill_Rasengun::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkill_Rasengun::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CGameObject* pRasenganCircle = pGameInstance->Add_GameObject(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_RasenganCircle");

	if (pRasenganCircle)
	{
		m_pRasengunCircle = dynamic_cast<CRasenganCircle*>(pRasenganCircle);
		CPlayer_Naruto* pOwner = dynamic_cast<CPlayer_Naruto*>(m_pSkillOwner);

		if(pOwner)
			m_pRasengunCircle->Set_Owner(pOwner);
	}

	Safe_Release(pGameInstance);

	Set_Active(false);


	m_fExplosionTime = 0.5f;
	m_fAttackTime = 1.0f;

	m_fEffectDelay = 0.1f;


	m_fAddHitTime = 0.05f;

	m_iAttack = 10;

    return S_OK;
}

void CSkill_Rasengun::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	
	if (m_bExplosion)
	{
		m_fCurrTime -= fTimeDelta;
		if (m_fCurrTime <= 0.0f)
		{
			m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(false);
			Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
			if (m_bAttackTime)
			{
				CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pSkillOwner);
				pPlayer->Get_Camera()->Cam_Shake(0.001f, 0.1f);
			}
				

			Set_Active(false);
		}
	}


	if (m_bEffect)
	{
		m_fEffectCurrTime += fTimeDelta;
		if (m_fEffectCurrTime >= m_fEffectDelay)
		{
			Effect_LineCircle();
			Effect_SmokeRing(0.5f, m_fSmokeRingScale);
			m_fSmokeRingScale += 0.1f;
			m_fEffectCurrTime = 0.0f;
		}


		m_fCurrHitTime += fTimeDelta;
		if (m_fCurrHitTime >= m_fAddHitTime)
		{
			Add_Hit();
			m_fCurrHitTime = 0.0f;
		}	
	}
}

void CSkill_Rasengun::LateTick(_float fTimeDelta)
{
	if (m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->IsActive())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CSkill_Rasengun::Render()
{
	if (m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->IsActive())
	{
		m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Center();
		m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->DebugRender();
	}
	return S_OK;
}

void CSkill_Rasengun::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	if (m_bAttackTime == false)
	{
		m_pSkillOwner->Set_SlowMotion(true);
		m_fCurrTime = m_fAttackTime;
		dynamic_cast<CPlayer*>(m_pSkillOwner)->Get_Camera()->Cam_Shake(0.001f, 10.0f);
		m_bEffect = true;
		m_bAttackTime = true;
	}
	++m_iHitCount;
}

void CSkill_Rasengun::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
	
}

void CSkill_Rasengun::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
	m_pSkillOwner->Set_SlowMotion(false);
}

HRESULT CSkill_Rasengun::Ready_Coliders()
{
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Radius(0.5f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Offset(Vec3(0.0f, 0.0f, 0.0f));


	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Radius(0.5f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Offset(Vec3(0.0f, 0.0f, 0.0f));
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_AttackCollider(m_iAttack, (_uint)COLLIDER_ATTACK::SPINBLOWUP, false);

	return S_OK;
}

void CSkill_Rasengun::Send_SkillInfo()
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


void CSkill_Rasengun::Explosion()
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pRasengunCircle->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));

	if(m_pSkillOwner->Is_Control())
		Send_Explosion();

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Center();


	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(true);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_AttackCollider(m_iAttack, (_uint)COLLIDER_ATTACK::SPINBLOWUP, true);
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);

	m_bExplosion = true;

	m_pRasengunCircle->DisAppear();

	Effect_SmokeRing(0.0f, m_fSmokeRingScale);
	Effect_SmokeRing(0.0f, m_fSmokeRingScale);
}

void CSkill_Rasengun::Appear()
{
	m_pRasengunCircle->Appear();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pRasengunCircle->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));

	Effect_SmokeRing(0.0f, 0.1f);
	Effect_SmokeRing(0.0f, 0.1f);

	Set_Active(true);
	m_bExplosion = false;
	m_fCurrTime = m_fExplosionTime;
	m_bAttackTime = false;
	m_bEffect = false;
	m_fEffectCurrTime = m_fEffectDelay;
	m_fSmokeRingScale = 0.5f;
	m_fCurrHitTime = 0.0f;

	m_iHitCount = 0;
}

void CSkill_Rasengun::Set_Run()
{
	m_pRasengunCircle->RunSmoke();
}



HRESULT CSkill_Rasengun::Ready_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
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

void CSkill_Rasengun::Effect_LineCircle()
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	Vec4 vColor(0.03f, 0.69f, 0.95f, 1.0f);

	vColor *= 1.2f;
	Vec4 vBlurColor(0.07f, 0.3f, 0.63f, 1.0f);
	vBlurColor *= 0.5f;

	for (_uint i = 0; i < 20; ++i)
	{
		CLineCircle* pLineCircle = CPool<CLineCircle>::Get_Obj();
		if (pLineCircle)
		{
			pLineCircle->Appear(vPos, vColor, vBlurColor, 1.0f);
		}
	}
}

void CSkill_Rasengun::Effect_SmokeRing(_float fTime, _float fBaseScale)
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	for (_uint i = 0; i < 8; ++i)
	{
		CSmokeRing* pSmokeRing = CPool<CSmokeRing>::Get_Obj();
		pSmokeRing->Appear(vPos, fBaseScale, Vec4(0.5f, 0.84f, 1.0f, 1.0f), Vec4(0.5f, 0.84f, 1.0f, 1.0f), fTime);
	}
}

void CSkill_Rasengun::Add_Hit()
{
	if (m_iHitCount == 0)
		return;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pSkillOwner);

	for (_uint i = 0; i < m_iHitCount; ++i)
	{
		pPlayer->Add_Hit();
	}

}



CSkill_Rasengun* CSkill_Rasengun::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Rasengun* pInstance = new CSkill_Rasengun(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		cout << "Failed To Created : CSkill_Rasengun" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_Rasengun::Clone(void* pArg)
{
	CSkill_Rasengun* pInstance = new CSkill_Rasengun(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CSkill_Rasengun" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Rasengun::Free()
{
	__super::Free();
}
