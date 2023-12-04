#include "stdafx.h"
#include "GameInstance.h"
#include "Skill_TwinBird.h"
#include "AsUtils.h"
#include "ColliderSphere.h"
#include "ServerSessionManager.h"
#include "RigidBody.h"
#include "Pool.h"
#include "Smoke_24.h"
#include "Explosion.h"
#include "ExplosionRing.h"
#include "Smoke_24.h"
#include "LineCircle.h"
#include "Player.h"
#include "Camera_Player.h"


CSkill_TwinBird::CSkill_TwinBird(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill(pDevice, pContext)
{
}

CSkill_TwinBird::CSkill_TwinBird(const CSkill_TwinBird& rhs)
	: CSkill(rhs)
{
}

HRESULT CSkill_TwinBird::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkill_TwinBird::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	m_pRigidBody->SetCompareGruond(false);
	m_pRigidBody->UseGravity(false);


	m_iFly_Loop = m_pModelCom->Find_AnimIndex(L"Fly_Loop");
	m_iFly_Start = m_pModelCom->Find_AnimIndex(L"Fly_Start");


	Set_Active(false);
    return S_OK;
}

void CSkill_TwinBird::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bFlyLoop == false && m_pModelCom->Get_CurrAnim() == m_iFly_Start)
	{
		if (m_pModelCom->Is_AnimationEnd(m_iFly_Start))
		{
			m_pModelCom->Reserve_NextAnimation(m_iFly_Loop, 0.1f, 0, 0);
			m_bFlyLoop = true;
		}
	}


	m_fCurrTime += fTimeDelta;

	if (m_bExplosion)
	{
		if (m_fCurrTime >= m_fExplosionTime)
		{
			Set_Die();
			Set_Active(false);
		}
		return;
	}
	else
	{
		m_fCurrTime += fTimeDelta;

		if (m_fCurrTime >= m_fTimer)
		{
			Explosion();
			return;
		}
	}

	if (m_fLookSpeed >= 3.0f)
		m_fLookSpeed = 3.0f;
	else
		m_fLookSpeed += 3.0f * fTimeDelta;


	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vec3 vTargetPos = m_pNearTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	vTargetPos.y += 0.5f;

	Vec3 vDir = vTargetPos - vPos;
	vDir.Normalize();


	m_pTransformCom->SetUp_Lerp(vDir, 3.0f, fTimeDelta);
	m_pTransformCom->LookAt_Lerp_ForLand(vDir, m_fLookSpeed, fTimeDelta);
	m_pTransformCom->Go_Straight(10.0f, fTimeDelta);


}

void CSkill_TwinBird::LateTick(_float fTimeDelta)
{
	if(m_bRender)
		m_PlayAnimation = std::async(&CModel::Play_Animation, m_pModelCom, fTimeDelta);

	__super::LateTick(fTimeDelta);

	/*m_fSendInfoTime += fTimeDelta;
	if (m_fSendInfoTime >= 0.05f)
	{
		m_fSendInfoTime = 0.0f;
		Send_SkillInfo();
	}*/

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
}

HRESULT CSkill_TwinBird::Render()
{
	if (nullptr == m_pModelCom || nullptr == m_pShaderCom)
		return S_OK;

	
	if (m_bRender)
	{
		m_PlayAnimation.get();

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
			return S_OK;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
			return S_OK;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
			return S_OK;

		m_pModelCom->SetUpAnimation_OnShader(m_pShaderCom);


		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return S_OK;

			/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
				return E_FAIL;*/


			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 4)))
				return S_OK;
		}

		Safe_Release(pGameInstance);
	}

	return S_OK;
}

HRESULT CSkill_TwinBird::Render_ShadowDepth()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return S_OK;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return S_OK;

	Matrix matLightVeiw = pGameInstance->Get_DirectionLightMatrix();

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &matLightVeiw)))
		return S_OK;



	m_pModelCom->SetUpAnimation_OnShader(m_pShaderCom);


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return S_OK;*/

			/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
				return E_FAIL;*/


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 3)))
			return S_OK;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

void CSkill_TwinBird::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::PLAYER)
		{
			if (m_bExplosion == false)
				Explosion();
		}
	}
}

void CSkill_TwinBird::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
	
}

void CSkill_TwinBird::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
}

HRESULT CSkill_TwinBird::Ready_Coliders()
{
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(true);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Radius(1.0f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Offset(Vec3(0.0f, 0.2f, 0.0f));


	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Radius(2.5f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Offset(Vec3(0.0f, 0.2f, 0.0f));
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_AttackCollider(1, (_uint)COLLIDER_ATTACK::SPINBLOWUP, false);

	return S_OK;
}

void CSkill_TwinBird::Send_SkillInfo()
{
	Protocol::S_OBJECTINFO pkt;

	auto tObject = pkt.add_tobject();

	tObject->set_iobjectid(m_iObjectID);
	tObject->set_ilevel(CGameInstance::GetInstance()->Get_CurrLevelIndex());
	tObject->set_ilayer((_uint)LAYER_TYPE::LAYER_MONSTER);


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

void CSkill_TwinBird::Shoot_TwinBird(CGameObject* pTargetObject, Vec3 vPos, Vec3 vLook)
{
	m_bActive = true;
	m_bRender = true;

	m_pNearTarget = pTargetObject;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->LookAt_ForLandObject(vPos + vLook);

	m_fTimer = 5.0f;
	m_fCurrTime = 0.0f;
	m_bExplosion = false;
	m_fLookSpeed = 0.0f;

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(true);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(false);

	m_pModelCom->Set_CurrAnim(m_iFly_Start);
	m_bFlyLoop = false;

	Effect_Smoke();
}

void CSkill_TwinBird::Effect_Smoke()
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.y += 0.5f;

	for (_uint i = 0; i < 50; ++i)
	{
		CSmoke_24* pSmoke = CPool<CSmoke_24>::Get_Obj();
		pSmoke->Appear(vPos, Vec4(1.0f, 1.0f, 1.0f, 0.85f), Vec2(1.0f, 1.0f), 0.0f, 0.02f, 0.00f);
	}
}

void CSkill_TwinBird::Effect_Explosion()
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.y += 0.5f;

	{
		_float fLength = 0.2f;
		for (_uint i = 0; i < 10; ++i)
		{
			for (_uint i = 0; i < 3; ++i)
			{
				CExplosion* pExplosion = CPool<CExplosion>::Get_Obj();
				pExplosion->Appear(0.05f, vPos, fLength, 0.0f);
			}
			fLength += 0.2f;
		}
	}


	{
		for (_uint i = 0; i < 30; ++i)
		{
			CSmoke_24* pSmoke = CPool<CSmoke_24>::Get_Obj();

			pSmoke->Appear(vPos, Vec4(0.0f, 0.0f, 0.0f, 0.7f), Vec2(1.0f, 1.0f), 0.01f, 0.02f, 0.05f);
		}
	}

	{
		Vec4 vColor(1.0f, 1.0f, 0.0f, 1.0f);
		Vec4 vBlurColor(1.0f, 0.3f, 0.0f, 1.0f);

		for (_uint i = 0; i < 100; ++i)
		{
			CLineCircle* pLineCircle = CPool<CLineCircle>::Get_Obj();
			if (pLineCircle)
			{
				pLineCircle->Appear(vPos, vColor, vBlurColor, 3.0f);
			}
		}
	}


	{
		CExplosionRing* pExplosionRing = CPool<CExplosionRing>::Get_Obj();
		pExplosionRing->Appear(vPos, Vec3(25.0f, 25.0f, 0.0f));
	}
	
	CServerSessionManager::GetInstance()->Get_Player()->Get_Camera()->Cam_Shake(0.001f, 0.2f);

	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(CGameInstance::GetInstance()->Get_RandomSoundKey(L"BombSound"), g_fVolume * 0.25f, vPos, 40.0f);
}

void CSkill_TwinBird::Explosion()
{
	m_bRender = false;
	m_bExplosion = true;

	m_fCurrTime = 0.0f;

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(true);

	Effect_Explosion();
}



HRESULT CSkill_TwinBird::Ready_Components()
{
	__super::Ready_Components();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	///* For.Com_Model */
	wstring strComName = L"Prototype_Component_Model_TwinBirds";
	if (FAILED(__super::Add_Component(pGameInstance->Get_CurrLevelIndex(), strComName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}



CSkill_TwinBird* CSkill_TwinBird::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_TwinBird* pInstance = new CSkill_TwinBird(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		cout << "Failed To Created : CSkill_TwinBird" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_TwinBird::Clone(void* pArg)
{
	CSkill_TwinBird* pInstance = new CSkill_TwinBird(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CSkill_TwinBird" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_TwinBird::Free()
{
	__super::Free();
}
