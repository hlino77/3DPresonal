#include "stdafx.h"
#include "GameInstance.h"
#include "Player.h"
#include "ServerSessionManager.h"
#include "ServerSession.h"
#include "Camera_Player.h"
#include "AsUtils.h"
#include "ColliderSphere.h"


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

    return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{

}

void CPlayer::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pModelCom->Play_Animation(fTimeDelta);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);


	
}

HRESULT CPlayer::Render()
{
	if (nullptr == m_pModelCom || nullptr == m_pShaderCom)
		return S_OK;

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


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
			return S_OK;
	}

	Safe_Release(pGameInstance);


    return S_OK;
}




void CPlayer::Find_NearTarget()
{
	m_pNearTarget = nullptr;
	m_pNearTarget = CGameInstance::GetInstance()->Find_NearGameObject(CGameInstance::GetInstance()->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_MONSTER, this);
}

Vec3 CPlayer::Make_StraightDir()
{
	Vec3 vCameraRight = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_RIGHT);
	Vec3 vCameraLook = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_LOOK);

	Vec3 vDir = vCameraRight.Cross(m_pTransformCom->Get_State(CTransform::STATE::STATE_UP));
	vDir.Normalize();

	return vDir;
}

Vec3 CPlayer::Make_RightDir()
{
	Vec3 vCameraRight = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_RIGHT);
	Vec3 vCameraLook = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_LOOK);

	Vec3 vDir = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_RIGHT);
	vDir.Normalize();

	return vDir;
}

Vec3 CPlayer::Make_BackDir()
{
	Vec3 vCameraRight = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_RIGHT);
	Vec3 vCameraLook = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_LOOK);

	Vec3 vDir = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_RIGHT).Cross(m_pTransformCom->Get_State(CTransform::STATE::STATE_UP));
	vDir.Normalize();

	return -vDir;
}

Vec3 CPlayer::Make_LeftDir()
{
	Vec3 vCameraRight = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_RIGHT);
	Vec3 vCameraLook = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE::STATE_LOOK);

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

HRESULT CPlayer::Ready_Components()
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

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	///* For.Com_State */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachine)))
		return E_FAIL;


	///* For.Com_Model */
	wstring strComName = L"Prototype_Component_Model_" + m_strObjectTag;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, strComName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	{
		CCollider::ColliderInfo tColliderInfo;
		tColliderInfo.m_bActive = true;
		tColliderInfo.m_iLayer = (_uint)LAYER_COLLIDER::LAYER_BODY;
		tColliderInfo.pOwner = this;
		CSphereCollider* pCollider = nullptr;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_SphereColider"), TEXT("Com_ColliderBody"), (CComponent**)&pCollider, &tColliderInfo)))
			return E_FAIL;
		if(pCollider)
			m_Coliders.emplace((_uint)LAYER_COLLIDER::LAYER_BODY, pCollider);
	}


	{
		CCollider::ColliderInfo tColliderInfo;
		tColliderInfo.m_bActive = false;
		tColliderInfo.m_iLayer = (_uint)LAYER_COLLIDER::LAYER_ATTACK;
		tColliderInfo.pOwner = this;
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

	auto tPlayer = pkt.mutable_tplayer();

	tPlayer->set_ilevel(pGameInstance->Get_CurrLevelIndex());
	tPlayer->set_ilayer((_uint)LAYER_TYPE::LAYER_PLAYER);
	tPlayer->set_iplayerid(m_iObjectID);

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
	
	auto vOffset = pkt.mutable_voffset();
	vOffset->Resize(3, 0.0f);
	Vec3 vColliderOffset = pCollider->Get_Offset();
	memcpy(vOffset->mutable_data(), &vColliderOffset, sizeof(Vec3));

	SendBufferRef pSendBuffer = CClientPacketHandler::MakeSendBuffer(pkt);
	CServerSessionManager::GetInstance()->Send(pSendBuffer);

	Safe_Release(pGameInstance);
}


void CPlayer::Set_State(const wstring& szName)
{
	m_pStateMachine->Change_State(szName);
	Send_State(szName);
}


void CPlayer::Reserve_Animation(_uint iAnimIndex, _float fChangeTime, _uint iStartFrame, _uint iChangeFrame)
{
	m_pModelCom->Reserve_NextAnimation(iAnimIndex, fChangeTime, iStartFrame, iChangeFrame);

	if (m_bControl)
		Send_Animation(iAnimIndex, fChangeTime, iStartFrame, iChangeFrame);
}






CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
