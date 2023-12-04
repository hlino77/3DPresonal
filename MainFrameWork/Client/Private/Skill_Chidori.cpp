#include "stdafx.h"
#include "GameInstance.h"
#include "Skill_Chidori.h"
#include "AsUtils.h"
#include "ColliderSphere.h"
#include "ServerSessionManager.h"
#include "RigidBody.h"
#include "Camera_Player.h"
#include "Pool.h"
#include "ChidoriCenter.h"
#include "Player_Sasuke.h"

#include "Lightning_A_01.h"
#include "Lightning_A_02.h"
#include "Lightning_B_01.h"
#include "Lightning_B_02.h"
#include "Lightning_B_03.h"
#include "Lightning_C_01.h"
#include "Lightning_Chidori.h"
#include "Pool.h"
#include "LightningTrail.h"

CSkill_Chidori::CSkill_Chidori(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill(pDevice, pContext)
{
}

CSkill_Chidori::CSkill_Chidori(const CSkill_Chidori& rhs)
	: CSkill(rhs)
{
}

HRESULT CSkill_Chidori::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkill_Chidori::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CGameObject* pChidoriCenter = pGameInstance->Add_GameObject(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Effect_ChidoriCenter");

	m_pChidoriCenter = dynamic_cast<CChidoriCenter*>(pChidoriCenter);

	if (m_pChidoriCenter == nullptr)
		return E_FAIL;

	CPlayer_Sasuke* pSasuke = dynamic_cast<CPlayer_Sasuke*>(m_pSkillOwner);

	if (pSasuke == nullptr)
		return E_FAIL;

	m_pChidoriCenter->Set_Owner(pSasuke);

	Safe_Release(pGameInstance);

	Set_Active(false);


	m_fExplosionTime = 0.5f;
	m_fAttackTime = 1.0f;

	m_fEffectDelay = 0.05f;

	m_iAttack = 10;


	if (FAILED(Ready_Trails()))
		return E_FAIL;

    return S_OK;
}

void CSkill_Chidori::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Vec3 vPos = m_pChidoriCenter->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	Vec3 vLook = m_pSkillOwner->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	vLook.Normalize();
	m_pTransformCom->LookAt(vPos + vLook);


	if (m_bExplosion)
	{
		m_fCurrTime -= fTimeDelta;
		if (m_fCurrTime <= 0.0f)
		{
			if (m_pSkillOwner->Is_Control())
			{
				m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(false);
				Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);

				if (m_bAttackTime)
					CamShake();
			}

			DisAppearTrail();


			if(m_bAttackTime)
				CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"ChidoriBombEnd.wav", g_fVolume * 0.3f, m_pTransformCom->Get_State(CTransform::STATE_POSITION), 40.0f);

			Set_Active(false);
		}
	}


	m_fEffectCurrTime += fTimeDelta;
	if (m_fEffectCurrTime >= m_fEffectDelay)
	{
		ChidoriEffect();
		Add_Hit();
		m_fEffectCurrTime = 0.0f;

		if (m_bAttackTime)
			CamShake();
	}


	if (m_eState == CHIDORISTATE::RUN)
	{
		Update_TrailPoint(fTimeDelta);
	}
}

void CSkill_Chidori::LateTick(_float fTimeDelta)
{
	if (m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->IsActive())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CSkill_Chidori::Render()
{
	return S_OK;
}

void CSkill_Chidori::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	if (pOther->Get_Owner()->Is_Invincible())
		return;

	if (m_bAttackTime == false)
	{
		m_pSkillOwner->Set_SlowMotion(true);
		m_fCurrTime = m_fAttackTime;
		m_bEffect = true;
		m_bAttackTime = true;
		m_eState = CHIDORISTATE::HITEXPLOSION;

		CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"ChidoriBomb.wav", g_fVolume * 0.3f, m_pTransformCom->Get_State(CTransform::STATE_POSITION), 40.0f);
		CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"ChidoriBomb_1.wav", g_fVolume * 0.2f, m_pTransformCom->Get_State(CTransform::STATE_POSITION), 40.0f);
	}
	++m_iHitCount;
}

void CSkill_Chidori::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
	
}

void CSkill_Chidori::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
	m_pSkillOwner->Set_SlowMotion(false);
}

HRESULT CSkill_Chidori::Ready_Coliders()
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

void CSkill_Chidori::Send_SkillInfo()
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


void CSkill_Chidori::Explosion()
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pChidoriCenter->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));

	if(m_pSkillOwner->Is_Control())
		Send_Explosion();

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Center();


	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(true);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_AttackCollider(m_iAttack, (_uint)COLLIDER_ATTACK::SPINBLOWUP, true);
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);

	m_bExplosion = true;

	m_pChidoriCenter->DisAppear();

	m_eState = CHIDORISTATE::EXPLOSION;


	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"ChidoriEnd.wav", g_fVolume * 0.4f, m_pTransformCom->Get_State(CTransform::STATE_POSITION), 40.0f);
}

void CSkill_Chidori::Appear()
{
	Set_Active(true);
	m_bExplosion = false;
	m_fCurrTime = m_fExplosionTime;
	m_bAttackTime = false;
	m_bEffect = false;
	m_fEffectCurrTime = m_fEffectDelay;

	m_pChidoriCenter->Appear();
	Vec3 vPos = m_pChidoriCenter->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_eState = CHIDORISTATE::CHARGE;

	m_iHitCount = 0;

	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"ChidoriStart.wav", g_fVolume * 0.15f, vPos, 40.0f);
	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"ChidoriStart_1.wav", g_fVolume * 0.08f, vPos, 40.0f);
	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"ChidoriStart_2.wav", g_fVolume * 0.15f, vPos, 40.0f);
}

void CSkill_Chidori::AppearTrail()
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_vTrailCurrPos[0] = Vec3(0.0f, 0.0f, 0.0f);

	_uint iIndex = 1;

	for (_int i = -1; i < 3;)
	{
		for (_int k = -1; k < 3;)
		{
			Vec3 vPos(i * 0.1f, k * 0.1f, 0.0f);
			m_vTrailCurrPos[iIndex] = vPos;
			m_vTrailTargetPos[iIndex] = -vPos;
			++iIndex;
			k += 2;
		}
		i += 2;
	}

	Matrix matWorld = m_pTransformCom->Get_WorldMatrix();

	for (_uint i = 0; i < 5; ++i)
	{
		*(m_pTrailPos[i]) = XMVector3TransformCoord(m_vTrailCurrPos[i], matWorld);
	}

	for (_uint i = 0; i < 5; ++i)
	{
		m_pTrail[i]->Appear();
	}
}

void CSkill_Chidori::DisAppearTrail()
{
	for (_uint i = 0; i < 5; ++i)
	{
		m_pTrail[i]->Disappear();
	}
}

void CSkill_Chidori::Update_TrailPoint(_float fTimeDelta)
{
	for (_uint i = 1; i < 5; ++i)
	{
		Vec3 vDir = m_vTrailTargetPos[i] - m_vTrailCurrPos[i];

		_float fDistance = vDir.Length();


		if (fDistance < 10.0f * fTimeDelta)
		{
			m_vTrailCurrPos[i] = m_vTrailTargetPos[i];
			New_TargetTrailPoint(i);
		}
		else
		{
			vDir.Normalize();
			m_vTrailCurrPos[i] += vDir * 10.0f * fTimeDelta;
		}
	}

	Matrix matWorld = m_pTransformCom->Get_WorldMatrix();

	for (_uint i = 0; i < 5; ++i)
	{
		*(m_pTrailPos[i]) = XMVector3TransformCoord(m_vTrailCurrPos[i], matWorld);
	}
}

void CSkill_Chidori::New_TargetTrailPoint(_uint iIndex)
{
	Vec3 vDir = m_vTrailCurrPos[0] - m_vTrailCurrPos[iIndex];

	_float fRandomLength = (rand() % 5 + 1) * 0.1f;

	vDir.Normalize();

	m_vTrailTargetPos[iIndex] = m_vTrailCurrPos[iIndex] + (vDir * fRandomLength);
}

void CSkill_Chidori::ChidoriEffect()
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	CPool<CLightning_C_01>::Get_Obj()->Appear(vPos);

	switch (m_eState)
	{
	case CHIDORISTATE::CHARGE:
	{
		_uint iRandomLightning = rand() % 5;
		Make_Lightning(vPos, iRandomLightning);
		break;
	}
	case CHIDORISTATE::RUN:
	{
		break;
	}

	case CHIDORISTATE::EXPLOSION:
	{
		for (_uint i = 0; i < 3; ++i)
		{
			_uint iRandomLightning = rand() % 5;
			Make_Lightning(vPos, iRandomLightning);
		}
		break;
	}

	case CHIDORISTATE::HITEXPLOSION:
	{
		for (_uint i = 0; i < 10; ++i)
		{
			_uint iRandomLightning = rand() % 5;
			Make_Lightning(vPos, iRandomLightning);
		}

		CPool<CLightning_Chidori>::Get_Obj()->Appear(vPos);
		break;
	}


	}
}

void CSkill_Chidori::Make_Lightning(Vec3 vPos, _uint iIndex)
{
	switch (iIndex)
	{
	case 0:
		CPool<CLightning_A_01>::Get_Obj()->Appear(vPos);
		break;
	case 1:
		CPool<CLightning_A_02>::Get_Obj()->Appear(vPos);
		break;
	case 2:
		CPool<CLightning_B_01>::Get_Obj()->Appear(vPos);
		break;
	case 3:
		CPool<CLightning_B_02>::Get_Obj()->Appear(vPos);
		break;
	case 4:
		CPool<CLightning_B_03>::Get_Obj()->Appear(vPos);
		break;
	}
}

void CSkill_Chidori::Set_ChidoriState(CHIDORISTATE eState)
{
	m_eState = eState;

	if (m_eState == CHIDORISTATE::RUN)
	{
		AppearTrail();
		CGameInstance::GetInstance()->Find_Stop_Sound(L"ChidoriStart_2.wav");
		CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"ChidoriRun.wav", g_fVolume * 0.4f, m_pTransformCom->Get_State(CTransform::STATE_POSITION), 40.0f);
	}
}



HRESULT CSkill_Chidori::Ready_Components()
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

HRESULT CSkill_Chidori::Ready_Trails()
{
	//Prototype_GameObject_Effect_LightningTrail

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	for (_uint i = 0; i < 5; ++i)
	{
		
		m_pTrail[i] = dynamic_cast<CLightningTrail*>(pGameInstance->Add_GameObject(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Effect_LightningTrail"));
		if (m_pTrail[i] == nullptr)
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}

		m_pTrailPos[i] = new Vec3();
		m_pTrail[i]->Set_TrailPos(m_pTrailPos[i]);
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

void CSkill_Chidori::Add_Hit()
{
	if (m_iHitCount == 0)
		return;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pSkillOwner);

	for (_uint i = 0; i < m_iHitCount; ++i)
	{
		pPlayer->Add_Hit();
	}
}

void CSkill_Chidori::CamShake()
{
	if (m_pSkillOwner->Is_Control())
		dynamic_cast<CPlayer*>(m_pSkillOwner)->Get_Camera()->Cam_Shake(0.001f, 0.1f);
	else
	{
		Vec3 vCameraOwnerPos = CServerSessionManager::GetInstance()->Get_Player()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float fDistance = (vCameraOwnerPos - vPos).Length();
		if (fDistance < 5.0f)
			CServerSessionManager::GetInstance()->Get_Player()->Get_Camera()->Cam_Shake(0.001f, 0.1f);
	}
}



CSkill_Chidori* CSkill_Chidori::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Chidori* pInstance = new CSkill_Chidori(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		cout << "Failed To Created : CSkill_Rasengun" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_Chidori::Clone(void* pArg)
{
	CSkill_Chidori* pInstance = new CSkill_Chidori(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CSkill_Rasengun" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Chidori::Free()
{
	__super::Free();
}
