#include "stdafx.h"
#include "GameInstance.h"
#include "Player.h"
#include "ServerSessionManager.h"
#include "ServerSession.h"
#include "Camera_Player.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext, L"Player", OBJ_TYPE::PLAYER),
	m_vTargetPos(Vec3(0.0f, 0.0f, 0.0f))
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs),
	m_matTargetWorld(rhs.m_matTargetWorld)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	m_matTargetWorld = XMMatrixIdentity();
    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	MODELDESC* Desc = static_cast<MODELDESC*>(pArg);
	m_strObjectTag = Desc->strFileName;
	m_bControl = Desc->bControl;
	m_iObjectID = Desc->iObjectID;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Sockets()))
		return E_FAIL;

	if (FAILED(Ready_PlayerParts()))
		return E_FAIL;

    return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{

	// Update_Weapon();

	for (auto& pPart : m_Parts)
		pPart->Tick(fTimeDelta);
}

void CPlayer::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pModelCom->Play_Animation(fTimeDelta);

	for (auto& pPart : m_Parts)
		pPart->LateTick(fTimeDelta);


	for (auto& pPart : m_Parts)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, pPart);

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

void CPlayer::Send_WorldMatrix()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_uint iCurrLevel = pGameInstance->Get_CurrLevelIndex();
	if (iCurrLevel >= LEVELID::LEVEL_LOADING)
	{
		Safe_Release(pGameInstance);
		return;
	}

	Protocol::S_MATRIX pkt;
	pkt.set_iobjectid(m_iObjectID);
	pkt.set_ilevel(iCurrLevel);
	pkt.set_ilayer((int32)LAYER_TYPE::LAYER_PLAYER);
	Make_WorldMatrix_Packet(pkt);


	SendBufferRef pBuffer = CClientPacketHandler::MakeSendBuffer(pkt);
	CServerSessionManager::GetInstance()->Get_ServerSession()->Send(pBuffer);

	Safe_Release(pGameInstance);
}

void CPlayer::Set_State(const wstring& szName)
{
	m_pStateMachine->Change_State(szName);
}

void CPlayer::Reserve_Animation(_uint iAnimIndex, _float fChangeTime, _uint iStartFrame, _uint iChangeFrame)
{
	Send_Animation(iAnimIndex, fChangeTime, iStartFrame, iChangeFrame);
	m_pModelCom->Reserve_NextAnimation(iAnimIndex, fChangeTime, iStartFrame, iChangeFrame);
}

void CPlayer::Make_WorldMatrix_Packet(Protocol::S_MATRIX& pkt)
{
	Matrix matWorld = m_pTransformCom->Get_WorldMatrix();

	for (_uint i = 0; i < 4; ++i)
	{
		for (_uint k = 0; k < 4; ++k)
		{
			pkt.add_matrix(matWorld.m[i][k]);
		}
	}
}

HRESULT CPlayer::Ready_Sockets()
{
	/*if (nullptr == m_pModelCom)
		return E_FAIL;

	CHierarchyNode* pWeaponSocket = m_pModelCom->Get_HierarchyNode("SWORD");
	if (nullptr == pWeaponSocket)
		return E_FAIL;

	m_Sockets.push_back(pWeaponSocket);*/

	return S_OK;
}

HRESULT CPlayer::Ready_PlayerParts()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Sword */
	//CGameObject* pGameObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Sword"));

	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//m_Parts.push_back(pGameObject);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CPlayer::Update_Weapon()
{
	//if (nullptr == m_Sockets[PART_WEAPON])
	//	return E_FAIL;

	///* 행렬. */
	///*_matrix			WeaponMatrix = 뼈의 스페이스 변환(OffsetMatrix)
	//	* 뼈의 행렬(CombinedTransformation)
	//	* 모델의 PivotMatrix * 프렐이어의월드행렬. ;*/

	//_matrix WeaponMatrix = m_Sockets[PART_WEAPON]->Get_OffSetMatrix()
	//	* m_Sockets[PART_WEAPON]->Get_CombinedTransformation()
	//	* m_pModelCom->Get_PivotMatrix()
	//	* m_pTransformCom->Get_WorldMatrix();

	//m_Parts[PART_WEAPON]->SetUp_State(WeaponMatrix);

	return S_OK;
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

	for (auto& pPart : m_Parts)
		Safe_Release(pPart);

	m_Parts.clear();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
