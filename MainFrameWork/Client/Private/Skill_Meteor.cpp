#include "stdafx.h"
#include "GameInstance.h"
#include "Skill_Meteor.h"
#include "AsUtils.h"
#include "ColliderSphere.h"
#include "ServerSessionManager.h"
#include "RigidBody.h"
#include "Meteor_HemiSphere.h"
#include "Meteor_Bottom.h"
#include "Meteor_Smoke.h"
#include "Meteor_Cylinder.h"
#include "Smoke_09.h"
#include "Smoke_19.h"
#include "LineCircle.h"
#include "Pool.h"
#include "Player.h"
#include "ExplosionRing.h"
#include "Camera_Player.h"
#include "Explosion.h"
#include "Smoke_24.h"

CSkill_Meteor::CSkill_Meteor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill(pDevice, pContext)
{
}

CSkill_Meteor::CSkill_Meteor(const CSkill_Meteor& rhs)
	: CSkill(rhs)
{
}

HRESULT CSkill_Meteor::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkill_Meteor::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	{
		m_pSmoke = dynamic_cast<CMeteor_Smoke*>(pGameInstance->GetInstance()->Add_GameObject(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Effect_Meteor_Smoke"));

		if (m_pSmoke == nullptr)
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}

		m_pSmoke->Set_Owner(this);
	}

	{
		m_pBottom = dynamic_cast<CMeteor_Bottom*>(pGameInstance->GetInstance()->Add_GameObject(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Effect_Meteor_Bottom"));

		if (m_pBottom == nullptr)
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}

		m_pBottom->Set_Owner(this);
	}



	{
		m_pHemiSphere = dynamic_cast<CMeteor_HemiSphere*>(pGameInstance->GetInstance()->Add_GameObject(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Effect_Meteor_HemiSphere"));

		if (m_pHemiSphere == nullptr)
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}

		m_pHemiSphere->Set_Owner(this);

	}

	{
		m_pCylinder = dynamic_cast<CMeteor_Cylinder*>(pGameInstance->GetInstance()->Add_GameObject(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Effect_Meteor_Cylinder"));

		if (m_pCylinder == nullptr)
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}

		m_pCylinder->Set_Owner(this);

	}


	m_vStartPos = Vec3(78.95f, 98.0f, -52.07f);
	m_vEffectPos = Vec3(78.95f, 28.0f, -52.07f);
	

	Safe_Release(pGameInstance);

	Set_Active(false);

	

	m_pTransformCom->Set_Scale(Vec3(20.0f, 20.0f, 20.0f));


    return S_OK;
}

void CSkill_Meteor::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	
	if (m_bExplosion)
	{
		Set_Active(false);

	}
	else
	{
		CServerSessionManager::GetInstance()->Get_Player()->Get_Camera()->Cam_Shake(0.0001f, 1.0f);
	}

	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.y -= 10.0f * fTimeDelta;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CSkill_Meteor::LateTick(_float fTimeDelta)
{
	if(m_bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CSkill_Meteor::Render()
{
	if (nullptr == m_pModelCom || nullptr == m_pShaderCom)
		return S_OK;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return S_OK;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return S_OK;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return S_OK;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return S_OK;

		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return S_OK;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

void CSkill_Meteor::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
}

void CSkill_Meteor::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
	
}

void CSkill_Meteor::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{

}

HRESULT CSkill_Meteor::Ready_Coliders()
{
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Radius(0.5f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Offset(Vec3(0.0f, 0.0f, 0.0f));


	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Radius(0.5f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Offset(Vec3(0.0f, 0.0f, 0.0f));
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_AttackCollider(0, (_uint)COLLIDER_ATTACK::SPINBLOWUP, false);

	return S_OK;
}

void CSkill_Meteor::Send_SkillInfo()
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


void CSkill_Meteor::Explosion()
{
	m_bExplosion = true;

	m_pHemiSphere->DisAppear();
	m_pBottom->DisAppear();
	m_pSmoke->DisAppear();


	

	for (_uint i = 0; i < 30; ++i)
	{
		Vec3 vPos = m_vEffectPos;

		vPos.x += rand() % 100 * 0.3f - 15.0f;
		vPos.y += rand() % 20 * 0.3f;
		vPos.z += rand() % 100 * 0.3f - 15.0f;

		CSmoke_09* pSmoke = CPool<CSmoke_09>::Get_Obj();
		pSmoke->Appear(vPos);
	}

	for (_uint i = 0; i < 15; ++i)
	{
		Vec3 vPos = m_vEffectPos;

		vPos.x += rand() % 50 * 0.3f - 7.5f;
		vPos.y += rand() % 20 * 0.3f + 4.0f;
		vPos.z += rand() % 50 * 0.3f - 7.5f;

		CSmoke_09* pSmoke = CPool<CSmoke_09>::Get_Obj();
		pSmoke->Appear(vPos);
	}


	for (_uint i = 0; i < 30; ++i)
	{
		Vec3 vPos = m_vEffectPos;

		vPos.x += rand() % 100 * 0.3f - 15.0f;
		vPos.y += rand() % 20 * 0.3f;
		vPos.z += rand() % 100 * 0.3f - 15.0f;

		CSmoke_19* pSmoke = CPool<CSmoke_19>::Get_Obj();
		pSmoke->Appear(vPos);
	}

	for (_uint i = 0; i < 15; ++i)
	{
		Vec3 vPos = m_vEffectPos;

		vPos.x += rand() % 50 * 0.3f - 7.5f;
		vPos.y += rand() % 20 * 0.3f + 4.0f;
		vPos.z += rand() % 50 * 0.3f - 7.5f;

		CSmoke_19* pSmoke = CPool<CSmoke_19>::Get_Obj();
		pSmoke->Appear(vPos);
	}



	{
		Vec4 vColor(0.0f, 0.0f, 0.0f, 1.0f);
		Vec4 vBlurColor(1.0f, 0.0f, 0.0f, 1.0f);

		for (_uint i = 0; i < 300; ++i)
		{
			Vec3 vPos = m_vEffectPos;

			CLineCircle* pLineCircle = CPool<CLineCircle>::Get_Obj();
			if (pLineCircle)
			{
				pLineCircle->Appear_OnlyUp(vPos, vColor, vBlurColor, 11.0f);
			}
		}
	}



	{
		_float fLength = 0.3f;
		for (_uint i = 0; i < 10; ++i)
		{
			for (_uint i = 0; i < 4; ++i)
			{
				CExplosion* pExplosion = CPool<CExplosion>::Get_Obj();
				pExplosion->Appear_Up(0.2f, m_vEffectPos, fLength, 0.0f);
			}
			fLength += 1.0f;
		}
	}


	{
		for (_uint i = 0; i < 30; ++i)
		{
			CSmoke_24* pSmoke = CPool<CSmoke_24>::Get_Obj();

			pSmoke->Appear_Up(m_vEffectPos, Vec4(0.0f, 0.0f, 0.0f, 0.7f), Vec2(2.0f, 2.0f), 0.01f, 0.1f, 0.05f);
		}
	}


	CExplosionRing* pExplosionRing = CPool<CExplosionRing>::Get_Obj();

	pExplosionRing->Appear(m_vEffectPos, Vec3(80.0f, 80.0f, 0.0f));

	CServerSessionManager::GetInstance()->Get_Player()->Get_Camera()->Cam_Shake(0.002f, 1.0f);
}

void CSkill_Meteor::Appear()
{
	Set_Active(true);


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vStartPos);
	
	m_bExplosion = false;


	m_pHemiSphere->Appear();
	m_pBottom->Appear();
	m_pSmoke->Appear();
	m_pCylinder->Appear();
}



HRESULT CSkill_Meteor::Ready_Components()
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


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	///* For.Com_Model */
	wstring strComName = L"Prototype_Component_Model_Meteor";
	if (FAILED(__super::Add_Component(LEVELID::LEVEL_STATIC, strComName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}



CSkill_Meteor* CSkill_Meteor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Meteor* pInstance = new CSkill_Meteor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		cout << "Failed To Created : CSkill_Meteor" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_Meteor::Clone(void* pArg)
{
	CSkill_Meteor* pInstance = new CSkill_Meteor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CSkill_Meteor" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Meteor::Free()
{
	__super::Free();
}
