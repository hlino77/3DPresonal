#include "stdafx.h"
#include "GameInstance.h"
#include "Player.h"
#include "ServerSessionManager.h"
#include "ServerSession.h"
#include "Camera_Player.h"
#include "AsUtils.h"
#include "ColliderSphere.h"
#include "RigidBody.h"
#include "ColliderOBB.h"
#include "CollisionManager.h"
#include "PickingMgr.h"
#include "Struct.pb.h"
#include "NavigationMgr.h"
#include "Skill.h"
#include "UI_Hits.h"
#include "FootTrail.h"
#include "Pool.h"
#include "Teleport.h"
#include "UI_OtherPlayer.h"
#include "WireTrail.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext, L"Player", OBJ_TYPE::PLAYER)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	
    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	MODELDESC* Desc = static_cast<MODELDESC*>(pArg);
	m_strObjectTag = Desc->strFileName;
	m_bControl = Desc->bControl;
	m_iObjectID = Desc->iObjectID;
	m_iLayer = Desc->iLayer;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pRigidBody->SetMass(2.0f);

	m_tCullingSphere.Radius = 2.0f;

	Reset_Triangle();


	if (m_bControl)
	{
		m_pHitUI = dynamic_cast<CUI_Hits*>(CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_CurrLevelIndex(), _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_Hits")));

		if (m_pHitUI == nullptr)
			return E_FAIL;
	}


	m_iFootBoneIndex[0] = m_pModelCom->Find_BoneIndex(L"LeftToeBase_end");
	m_iFootBoneIndex[1] = m_pModelCom->Find_BoneIndex(L"RightToeBase_end");


	m_iHp = 100;
	m_iMaxHp = 100;

	

	if (FAILED(Ready_WireTrail()))
		return E_FAIL;

    return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	//if (KEY_TAP(KEY::SPACE))
	//{
	//	m_pRigidBody->AddForce(Vec3(0.0f, 1000.0f, 0.0f), ForceMode::FORCE);
	//	m_pRigidBody->UseGravity(true);
	//}

	if (KEY_TAP(KEY::I))
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, Vec3(73.43f, 27.93f, -37.87f));
	}

	if (KEY_TAP(KEY::F1))
	{
		Set_State(L"Idle");
	}

	if (m_fVoiceSoundDelay > 0.0f)
	{
		m_fVoiceSoundDelay = max(m_fVoiceSoundDelay - fTimeDelta, 0.0f);
	}

	if(!m_bWall && m_bNavi)
		CNavigationMgr::GetInstance()->SetUp_OnCell(this);

	m_pRigidBody->Tick(fTimeDelta);


	for (auto& Skill : m_SkillInfo)
	{
		Update_Skill(Skill, fTimeDelta);
	}


	if (m_bWall && m_bControl)
		Set_PlayerToWall(fTimeDelta);
	else if (m_bSetDefaultUp && m_bWall == false)
		Set_PlayerUp(fTimeDelta);
}

void CPlayer::LateTick(_float fTimeDelta)
{
	m_PlayAnimation = std::async(&CModel::Play_Animation, m_pModelCom, fTimeDelta * m_fAnimationSpeed);

	if (nullptr == m_pRendererCom)
		return;
	{
		READ_LOCK
		for (auto& CollisionStay : m_CollisionList)
			OnCollisionStay(CollisionStay.iColLayer, CollisionStay.pCollider);
	}
	

	//m_pModelCom->Play_Animation(fTimeDelta);

	CullingObject();

	if (m_bControl)
	{
		Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		CGameInstance::GetInstance()->Update_LightMatrix(vPos);
	}

}

HRESULT CPlayer::Render()
{
	if (nullptr == m_pModelCom || nullptr == m_pShaderCom)
		return S_OK;
	
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

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
		{
			if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
				return S_OK;
		}
		else
		{
			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 2)))
				return S_OK;
		}
	}

	Safe_Release(pGameInstance);


    return S_OK;
}

HRESULT CPlayer::Render_ShadowDepth()
{
	if (nullptr == m_pModelCom || nullptr == m_pShaderCom)
		return S_OK;

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




void CPlayer::Find_NearTarget()
{
	m_pNearTarget = nullptr;
	CGameObject* pBoss = CGameInstance::GetInstance()->Find_NearGameObject(CGameInstance::GetInstance()->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_BOSS, this);

	if (pBoss)
		m_pNearTarget = pBoss;
	else
	{
		m_pNearTarget = CGameInstance::GetInstance()->Find_NearGameObject(CGameInstance::GetInstance()->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_MONSTER, this);
	}
}

void CPlayer::Send_NearTarget()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Protocol::S_NEARTARGET pkt;

	pkt.set_ilevel(pGameInstance->Get_CurrLevelIndex());
	pkt.set_iobjectid(m_iObjectID);
	pkt.set_ilayer(m_iLayer);

	if (m_pNearTarget)
	{
		pkt.set_itargetobjectid(m_pNearTarget->Get_ObjectID());
		pkt.set_itargetobjectlayer(m_pNearTarget->Get_ObjectLayer());
	}
	else
	{
		pkt.set_itargetobjectid(-1);
		pkt.set_itargetobjectlayer(-1);
	}


	SendBufferRef pSendBuffer = CClientPacketHandler::MakeSendBuffer(pkt);
	CServerSessionManager::GetInstance()->Send(pSendBuffer);

	Safe_Release(pGameInstance);

}

Vec3 CPlayer::Make_StraightDir()
{
	Vec3 vCameraLook = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_LOOK);
	Vec3 vUp = m_pTransformCom->Get_State(CTransform::STATE::STATE_UP);
	vUp.Normalize();

	Vec3 vRight = vUp.Cross(vCameraLook);
	Vec3 vDir = vRight.Cross(vUp);
	vDir.Normalize();

	return vDir;
}

Vec3 CPlayer::Make_RightDir()
{
	Vec3 vCameraRight = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_RIGHT);

	Vec3 vDir = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_RIGHT);
	vDir.Normalize();

	return vDir;
}

Vec3 CPlayer::Make_BackDir()
{
	Vec3 vCameraLook = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_LOOK);
	Vec3 vUp = m_pTransformCom->Get_State(CTransform::STATE::STATE_UP);
	vUp.Normalize();

	Vec3 vRight = vUp.Cross(vCameraLook);
	Vec3 vDir = vRight.Cross(vUp);
	vDir.Normalize();

	return -vDir;
}

Vec3 CPlayer::Make_LeftDir()
{
	Vec3 vCameraRight = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_RIGHT);

	Vec3 vDir = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_RIGHT);
	vDir.Normalize();

	return -vDir;
}

Vec3 CPlayer::Make_Straight_JumpDir()
{
	Vec3 vCameraRight = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_RIGHT);

	Vec3 vDir = vCameraRight.Cross(Vec3(0.0f, 1.0f, 0.0f));
	vDir.Normalize();

	return vDir;
}

Vec3 CPlayer::Make_Right_JumpDir()
{
	Vec3 vCameraRight = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_RIGHT);

	Vec3 vDir = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_RIGHT);
	vDir.Normalize();

	return vDir;
}

Vec3 CPlayer::Make_Back_JumpDir()
{
	Vec3 vCameraRight = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_RIGHT);

	Vec3 vDir = vCameraRight.Cross(Vec3(0.0f, 1.0f, 0.0f));
	vDir.Normalize();

	return -vDir;
}

Vec3 CPlayer::Make_Left_JumpDir()
{
	Vec3 vCameraRight = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_RIGHT);

	Vec3 vDir = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_RIGHT);
	vDir.Normalize();

	return -vDir;
}

void CPlayer::Go_Straight(_float fSpeed, _float fTimeDelta)
{
	m_pTransformCom->Go_Straight(fSpeed, fTimeDelta);
}

void CPlayer::Move_Dir(Vec3 vDir, _float fSpeed, _float fTimeDelta)
{
	m_pTransformCom->LookAt_Lerp(vDir, 5.0f, fTimeDelta);
	m_pTransformCom->Go_Straight(fSpeed, fTimeDelta);
}


void CPlayer::Follow_ServerPos(_float fDistance, _float fLerpSpeed)
{
	Vec3 vCurrPos = m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);
	Vec3 vCurrUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	Matrix matTargetWorld = m_matTargetWorld;

	{
		Vec3 vServerPos(matTargetWorld.m[3]);

		Vec3 vDistance = vServerPos - vCurrPos;
		if (vDistance.Length() > fDistance)
		{
			vCurrPos = Vec3::Lerp(vCurrPos, vServerPos, fLerpSpeed);
			m_pTransformCom->Set_State(CTransform::STATE::STATE_POSITION, vCurrPos);
		}
	}
	

	{
		Vec3 vServerUp(matTargetWorld.m[1]);

		Vec3 vDistance = vServerUp - vCurrUp;
		if (vDistance.Length() > 0.0001f)
		{
			vCurrUp = Vec3::Lerp(vCurrUp, vServerUp, fLerpSpeed * 1.5f);
			m_pTransformCom->Set_Up(vCurrUp);
		}
	}

}


void CPlayer::Set_PlayerToWall(_float fTimeDelta)
{
	Vec3 vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	vUp.Normalize();
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vec3 vRayPos = vPos + vUp * 5.0f;
	Vec3 vDir = -vUp;

	CPickingMgr::GetInstance()->Set_Ray(vRayPos, vDir);


	if (m_tTriangle.fDist == -1.0f)
		return;

	_float fDist;
	if (TriangleTests::Intersects(vRayPos, vDir, m_tTriangle.vPos0, m_tTriangle.vPos1, m_tTriangle.vPos2, fDist))
	{
		Vec3 vTargetPos = vRayPos + vDir * fDist;
		vTargetPos += -vDir * 0.05f;

		vPos = Vec3::Lerp(vPos, vTargetPos, 8.0f * fTimeDelta);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		vUp = Vec3::Lerp(vUp, m_tTriangle.vNormal, 8.0f * fTimeDelta);

		m_pTransformCom->Set_Up(vUp);

		_float fAngle = XMConvertToDegrees(acosf(m_tTriangle.vNormal.Dot(Vec3(0.0f, 1.0f, 0.0f))));

		if (fAngle < 45.0f)
		{
			DisAppear_FootTrail();
			m_bWall = false;
			m_bGravity = true;
		}
	}

}

void CPlayer::Set_PlayerUp(_float fTimeDelta)
{
	Vec3 vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	vUp.Normalize();
	Vec3 vTargetUp(0.0f, 1.0f, 0.0f);


	if (vUp.Dot(vTargetUp) != 1.0f)
	{
		Vec3 vDir = vTargetUp - vUp;

		_float fDistance = vDir.Length();
		_float fSpeed = 8.0f * fTimeDelta;

		if (fDistance <= fSpeed)
		{
			vUp = vTargetUp;
		}
		else
		{
			vDir.Normalize();
			vUp += vDir * fSpeed;
		}


		m_pTransformCom->Set_Up(vUp);
	}
	

}

void CPlayer::Body_Collision(CGameObject* pObject)
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vec3 vOtherPos = pObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	Vec3 vDir = vPos - vOtherPos;
	_float fDistance = vDir.Length();


	if (fDistance < 0.5f)
	{
		vDir.Normalize();
		Vec3 vTargetPos = vOtherPos + vDir * 0.5f;
		vPos = Vec3::Lerp(vPos, vTargetPos, 0.2f);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}
}

void CPlayer::Hit_Attack(CCollider* pCollider)
{
	CGameObject* pOwner = pCollider->Get_Owner();

	_uint iObjType = pOwner->Get_ObjectType();
	
	if (iObjType == OBJ_TYPE::PLAYER)
		return;

	if (iObjType == OBJ_TYPE::SKILL)
	{
		_uint iSkillOwnerType = dynamic_cast<CSkill*>(pOwner)->Get_SkillOwner()->Get_ObjectType();
		if (iSkillOwnerType == OBJ_TYPE::PLAYER)
			return;
	}

	m_iHp -= pCollider->Get_Attack();
	if (m_iHp < 0)
		m_iHp = 0;


	if (m_iHp == 0)
		m_iHp = 100;

	Send_Hp();

	_uint iAttackType = pCollider->Get_AttackType();


	switch (iAttackType)
	{
	case (_uint)COLLIDER_ATTACK::MIDDLE:
		m_pHitObject = pCollider->Get_Owner();
		Set_State(L"Hit_Middle");
		if(iObjType != OBJ_TYPE::SKILL)
			m_pCamera->Cam_Shake(0.001f, 0.1f);
		break;
	case (_uint)COLLIDER_ATTACK::SPINBLOWUP:
		m_pHitObject = pCollider->Get_Owner();
		Set_State(L"Hit_SpinBlowUp");
		if (iObjType != OBJ_TYPE::SKILL)
			m_pCamera->Cam_Shake(0.002f, 0.15f);
		break;
	}
	
	Set_SlowMotion(pCollider->Get_SlowMotion());
}

void CPlayer::Add_Hit()
{
	if (m_bControl)
		m_pHitUI->Add_Hit();
}

void CPlayer::Set_SlowMotion(_bool bSlow)
{
	if (bSlow)
	{
		if (m_iSlowMotionCount <= 0)
		{
			m_fAttackMoveSpeed = 0.1f;
			m_fAnimationSpeed = 0.01f;
			m_pRigidBody->Set_Active(false);
		}

		m_iSlowMotionCount++;
	}
	else
	{
		--m_iSlowMotionCount;

		if (m_iSlowMotionCount <= 0)
		{
			m_fAttackMoveSpeed = 8.0f;
			m_fAnimationSpeed = 1.0f;
			m_pRigidBody->Set_Active(true);
			m_iSlowMotionCount = 0;
		}
	}

	if(m_bControl)
		Send_SlowMotion(bSlow);
}

void CPlayer::Effect_Teleport()
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vec3 vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	vUp.Normalize();

	vPos += vUp * 0.5f;

	CTeleport* pTeleport = CPool<CTeleport>::Get_Obj();
	pTeleport->Appear(vPos, vUp);
}

HRESULT CPlayer::Ready_UI_OtherPlayer()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_OtherPlayer* pUI = dynamic_cast<CUI_OtherPlayer*>(pGameInstance->Add_GameObject(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_UI, L"Prototype_GameObject_UI_OtherPlayer", this));

	if (pUI == nullptr)
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	pUI->Appear();

	Safe_Release(pGameInstance);

}

void CPlayer::Set_NickName(const wstring& szNickName)
{
	m_szNickName = szNickName;

	if(m_bControl == false)
		Ready_UI_OtherPlayer();
}



HRESULT CPlayer::Ready_WireTrail()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	CWireTrail::WireTrailDesc tDesc;
	tDesc.pOwner = this;
	tDesc.iBoneIndex = m_pModelCom->Find_BoneIndex(L"RightHandThumb3_end");

	CWireTrail* pTrail = dynamic_cast<CWireTrail*>(pGameInstance->Add_GameObject(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Effect_WireTrail", &tDesc));

	if (pTrail == nullptr)
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


	m_pWireTrail = pTrail;

	Safe_Release(pGameInstance);


	return S_OK;
}


_bool CPlayer::Stop_VoiceSound()
{
	if (m_fVoiceSoundDelay == 0.0f)
	{
		CGameInstance::GetInstance()->Find_Stop_Sound(m_VoiceSoundKey);
		return true;
	}

	return false;
}

HRESULT CPlayer::Ready_Components()
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

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	///* For.Com_State */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachine)))
		return E_FAIL;

	///* For.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"), TEXT("Com_RigidBody"), (CComponent**)&m_pRigidBody)))
		return E_FAIL;

	///* For.Com_Model */
	wstring strComName = L"Prototype_Component_Model_" + m_strObjectTag;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strComName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	{
		CCollider::ColliderInfo tColliderInfo;
		tColliderInfo.m_bActive = true;
		tColliderInfo.m_iLayer = (_uint)LAYER_COLLIDER::LAYER_BODY;
		CSphereCollider* pCollider = nullptr;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_SphereColider"), TEXT("Com_ColliderBody"), (CComponent**)&pCollider, &tColliderInfo)))
			return E_FAIL;
		if(pCollider)
			m_Coliders.emplace((_uint)LAYER_COLLIDER::LAYER_BODY, pCollider);

		CCollisionManager::GetInstance()->Add_Colider(m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]);
	}


	{
		CCollider::ColliderInfo tColliderInfo;
		tColliderInfo.m_bActive = false;
		tColliderInfo.m_iLayer = (_uint)LAYER_COLLIDER::LAYER_ATTACK;
		CSphereCollider* pCollider = nullptr;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_SphereColider"), TEXT("Com_ColliderAttack"), (CComponent**)&pCollider, &tColliderInfo)))
			return E_FAIL;
		if(pCollider)
			m_Coliders.emplace((_uint)LAYER_COLLIDER::LAYER_ATTACK, pCollider);
	}



	Safe_Release(pGameInstance);

	Vec3 vScale;
	vScale.x = 0.01f;
	vScale.y = 0.01f;
	vScale.z = 0.01f;

	m_pTransformCom->Set_Scale(vScale);

    return S_OK;
}

void CPlayer::CullingObject()
{
	if (m_bControl)
	{
		if (m_bRender)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		}
			
		return;
	}

	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_tCullingSphere.Center = vPos;

	const BoundingFrustum& tCamFrustum = CGameInstance::GetInstance()->Get_CamFrustum();

	if (tCamFrustum.Intersects(m_tCullingSphere) == false)
		return;

	if (m_bRender)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
	}
		
}

void CPlayer::Update_Skill(SKILLINFO& tSkill, _float fTimeDelta)
{
	if (tSkill.m_bReady == false)
	{
		tSkill.m_fCurrCoolTime += fTimeDelta;

		if (tSkill.m_fCurrCoolTime > tSkill.m_fCoolTime)
		{
			tSkill.m_fCurrCoolTime = 0.0f;
			tSkill.m_bReady = true;
		}
	}
}

void CPlayer::Appear_FootTrail()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);



	for (_uint i = 0; i < 2; ++i)
	{
		if (m_pFootTrail[i] == nullptr)
		{
			m_pFootTrail[i] = CPool<CFootTrail>::Get_Obj();
			m_pFootTrail[i]->Appear(this, m_iFootBoneIndex[i], Vec4(0.035f, 0.69f, 0.95f, 1.0f), Vec4(0.074f, 0.24f, 0.58f, 1.0f));
			//m_pFootTrail[i]->Appear(this, m_iFootBoneIndex[i], Vec4(0.0f, 0.0f, 1.0f, 1.0f));
			//m_pFootTrail[i]->Appear(this, m_iFootBoneIndex[i], Vec4(1.0f, 0.0f, 0.0f, 1.0f), Vec4(0.074f, 0.24f, 0.58f, 1.0f));
		}
	}


	Safe_Release(pGameInstance);

}

void CPlayer::DisAppear_FootTrail()
{
	for (_uint i = 0; i < 2; ++i)
	{
		if (m_pFootTrail[i])
		{
			m_pFootTrail[i]->Disappear();
			m_pFootTrail[i] = nullptr;
		}
	}
}


void CPlayer::Send_Animation(_uint iAnimIndex, _float fChangeTime, _uint iStartFrame, _uint iChangeFrame)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_uint iCurrLevel = pGameInstance->Get_CurrLevelIndex();
	if (iCurrLevel >= LEVELID::LEVEL_LOADING)
	{
		Safe_Release(pGameInstance);
		return;
	}
	
	Protocol::S_ANIMATION pkt;
	pkt.set_iobjectid(m_iObjectID);
	pkt.set_ilevel(iCurrLevel);
	pkt.set_ilayer((int32)LAYER_TYPE::LAYER_PLAYER);
	pkt.set_ianimindex(iAnimIndex);
	pkt.set_fchangetime(fChangeTime);
	pkt.set_ichangeframe(iChangeFrame);
	pkt.set_istartframe(iStartFrame);

	SendBufferRef pBuffer = CClientPacketHandler::MakeSendBuffer(pkt);
	CServerSessionManager::GetInstance()->Get_ServerSession()->Send(pBuffer);

	Safe_Release(pGameInstance);
}

void CPlayer::Send_State(const wstring& szName)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Protocol::S_STATE pkt;
	pkt.set_strstate(CAsUtils::ToString(szName));

	auto tPlayer = pkt.mutable_tobject();

	tPlayer->set_ilevel(pGameInstance->Get_CurrLevelIndex());
	tPlayer->set_ilayer((_uint)LAYER_TYPE::LAYER_PLAYER);
	tPlayer->set_iobjectid(m_iObjectID);

	auto vTargetPos = tPlayer->mutable_vtargetpos();
	vTargetPos->Resize(3, 0.0f);
	Vec3 vPlayerTargetPos = m_vTargetPos.load();
	memcpy(vTargetPos->mutable_data(), &vPlayerTargetPos, sizeof(Vec3));


	auto matWorld = tPlayer->mutable_matworld();
	matWorld->Resize(16, 0.0f);
	Matrix matPlayerWorld = m_pTransformCom->Get_WorldMatrix();
	memcpy(matWorld->mutable_data(), &matPlayerWorld, sizeof(Matrix));

	if (m_pNearTarget == nullptr)
		pkt.set_itargetobjectid(-1);
	else
	{
		pkt.set_itargetobjectid(m_pNearTarget->Get_ObjectID());
		pkt.set_itargetobjectlayer(m_pNearTarget->Get_ObjectLayer());
	}

	if (m_pHitObject == nullptr)
		pkt.set_ihitobjectid(-1);
	else
	{
		pkt.set_ihitobjectid(m_pHitObject->Get_ObjectID());
		pkt.set_ihitobjectlayer(m_pHitObject->Get_ObjectLayer());
	}
		

	SendBufferRef pSendBuffer = CClientPacketHandler::MakeSendBuffer(pkt);
	CServerSessionManager::GetInstance()->Send(pSendBuffer);

	Safe_Release(pGameInstance);
}

void CPlayer::Send_ColliderState(const _uint& iLayer)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CSphereCollider* pCollider = m_Coliders[iLayer];

	Protocol::S_COLLIDERSTATE pkt;
	pkt.set_iobjectid(m_iObjectID);
	pkt.set_ilevel(pGameInstance->Get_CurrLevelIndex());
	pkt.set_ilayer(m_iLayer);

	pkt.set_icollayer(pCollider->Get_ColLayer());
	pkt.set_bactive(pCollider->IsActive());
	pkt.set_fradius(pCollider->Get_Radius());
	pkt.set_iattacktype(pCollider->Get_AttackType());
	pkt.set_iattack(pCollider->Get_Attack());
	pkt.set_bslow(pCollider->Get_SlowMotion());

	auto vOffset = pkt.mutable_voffset();
	vOffset->Resize(3, 0.0f);
	Vec3 vColliderOffset = pCollider->Get_Offset();
	memcpy(vOffset->mutable_data(), &vColliderOffset, sizeof(Vec3));

	

	if (pCollider->Get_Child())
	{
		COBBCollider* pChild = dynamic_cast<COBBCollider*>(pCollider->Get_Child());
		auto tchild = pkt.add_tchild();


		auto vOffset = tchild->mutable_voffset();
		vOffset->Resize(3, 0.0f);
		Vec3 vColOffset = pChild->Get_Offset();

		memcpy(vOffset->mutable_data(), &vColOffset, sizeof(Vec3));


		auto vScale = tchild->mutable_vscale();
		vScale->Resize(3, 0.0f);
		Vec3 vColScale = pChild->Get_Scale();

		memcpy(vScale->mutable_data(), &vColScale, sizeof(Vec3));
	}

	SendBufferRef pSendBuffer = CClientPacketHandler::MakeSendBuffer(pkt);
	CServerSessionManager::GetInstance()->Send(pSendBuffer);

	Safe_Release(pGameInstance);
}

void CPlayer::Send_SlowMotion(_bool bSlow)
{
	Protocol::S_SLOWMOTION pkt;


	pkt.set_ilevel(CGameInstance::GetInstance()->Get_CurrLevelIndex());
	pkt.set_ilayer(m_iLayer);
	pkt.set_iobjectid(m_iObjectID);
	pkt.set_bslow(bSlow);


	SendBufferRef pSendBuffer = CClientPacketHandler::MakeSendBuffer(pkt);
	CServerSessionManager::GetInstance()->Send(pSendBuffer);
}

void CPlayer::Send_MakeSkill(const wstring& szSkillName)
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Protocol::S_SETSKILL pkt;

	pkt.set_ilevel(pGameInstance->Get_CurrLevelIndex());
	pkt.set_ilayer((_uint)LAYER_TYPE::LAYER_PLAYER);
	pkt.set_iobjectid(m_iObjectID);

	pkt.set_szskillname(CAsUtils::ToString(szSkillName));

	SendBufferRef pSendBuffer = CClientPacketHandler::MakeSendBuffer(pkt);
	CServerSessionManager::GetInstance()->Send(pSendBuffer);

	Safe_Release(pGameInstance);
}

void CPlayer::Send_Hp()
{
	Protocol::S_HP pkt;

	pkt.set_ilevel(CGameInstance::GetInstance()->Get_CurrLevelIndex());
	pkt.set_ilayer(m_iLayer);
	pkt.set_iobjectid(m_iObjectID);
	pkt.set_ihp(m_iHp);

	SendBufferRef pSendBuffer = CClientPacketHandler::MakeSendBuffer(pkt);
	CServerSessionManager::GetInstance()->Send(pSendBuffer);
}


void CPlayer::Set_State(const wstring& szName)
{
	m_pStateMachine->Change_State(szName);
	Send_State(szName);
}


void CPlayer::Reserve_Animation(_uint iAnimIndex, _float fChangeTime, _uint iStartFrame, _uint iChangeFrame)
{
	m_pModelCom->Reserve_NextAnimation(iAnimIndex, fChangeTime, iStartFrame, iChangeFrame);

	//if (m_bControl)
		//Send_Animation(iAnimIndex, fChangeTime, iStartFrame, iChangeFrame);
}




void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
