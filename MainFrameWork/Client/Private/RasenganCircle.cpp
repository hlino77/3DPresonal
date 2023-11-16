#include "stdafx.h"
#include "RasenganCircle.h"
#include "Client_Defines.h"
#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
#include "VIBuffer_Point.h"
#include "RigidBody.h"
#include "Pool.h"
#include "Player_Naruto.h"
#include "Rasengan_StartSmoke.h"
#include "Rasengan_RunSmoke.h"

CRasenganCircle::CRasenganCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CRasenganCircle::CRasenganCircle(const CRasenganCircle& rhs)
	: CEffect(rhs)
{
}

HRESULT CRasenganCircle::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CRasenganCircle::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	m_szModelName = L"Effect_RasenganCircle";

	m_bActive = false;


	m_vColor = Vec4(0.035f, 0.69f, 0.95f, 1.0f);
	m_vBlurColor = Vec4(0.0f, 0.0f, 1.0f, 1.0f);

	m_vBlurColor *= 0.2f;

	m_vScale = Vec3(0.2f, 0.2f, 0.2f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Vec3(0.009f, 0.039f, 0.089f));



	m_matOrigin = m_pTransformCom->Get_WorldMatrix();


	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		//Prototype_GameObject_Effect_Rasengan_StartSmoke
		CRasengan_StartSmoke* pStartSmoke = dynamic_cast<CRasengan_StartSmoke*>(pGameInstance->Add_GameObject(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Effect_Rasengan_StartSmoke"));

		if (pStartSmoke == nullptr)
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}
		

		m_pStartSmoke = pStartSmoke;
		m_pStartSmoke->Set_Owner(this);

		Safe_Release(pGameInstance);
	}

	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		//Prototype_GameObject_Effect_Rasengan_StartSmoke
		CRasengan_RunSmoke* pRunSmoke = dynamic_cast<CRasengan_RunSmoke*>(pGameInstance->Add_GameObject(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Effect_Rasengan_RunSmoke"));

		if (pRunSmoke == nullptr)
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}


		m_pRunSmoke = pRunSmoke;
		m_pRunSmoke->Set_Owner(this);

		Safe_Release(pGameInstance);
	}



	return S_OK;
}

void CRasenganCircle::Tick(_float fTimeDelta)
{
	//Vec3 vPos = m_matOrigin.Translation();


	//if (KEY_TAP(KEY::LEFT_ARROW))
	//{
	//	vPos.x += 0.01f;
	//}
	//if (KEY_TAP(KEY::RIGHT_ARROW))
	//{
	//	vPos.x -= 0.01f;
	//}
	//if (KEY_TAP(KEY::UP_ARROW))
	//{
	//	vPos.y += 0.01f;
	//}
	//if (KEY_TAP(KEY::DOWN_ARROW))
	//{
	//	vPos.y -= 0.01f;
	//}
	//if (KEY_TAP(KEY::M))
	//{
	//	vPos.z += 0.01f;
	//}
	//if (KEY_TAP(KEY::N))
	//{
	//	vPos.z -= 0.01f;
	//}

	//m_matOrigin.Translation(vPos);

	if (m_bEnd)
	{
		if (m_matOrigin._11 > 0.001f)
		{
			m_matOrigin._11 -= 0.5f * fTimeDelta;

			if (m_matOrigin._11 < 0.001f)
				m_matOrigin._11 = 0.001f;
		}

		if (m_matOrigin._22 > 0.001f)
		{
			m_matOrigin._22 -= 0.5f * fTimeDelta;

			if (m_matOrigin._22 < 0.001f)
				m_matOrigin._22 = 0.001f;
		}


		if (m_matOrigin._11 == 0.001f && m_matOrigin._22 == 0.001f)
		{
			Set_Active(false);
			return;
		}
	}
	else
	{
		if (m_matOrigin._11 < m_vScale.x)
		{
			m_matOrigin._11 += 0.5f * fTimeDelta;

			if (m_matOrigin._11 > m_vScale.x)
				m_matOrigin._11 = m_vScale.x;
		}

		if (m_matOrigin._22 < m_vScale.y)
		{
			m_matOrigin._22 += 0.5f * fTimeDelta;

			if (m_matOrigin._22 > m_vScale.y)
				m_matOrigin._22 = m_vScale.y;
		}

	}

	Matrix matBone = m_pPlayer->Get_ModelCom()->Get_CurrBoneMatrix(m_iBoneIndex);
	Matrix matPlayerWorld = m_pPlayer->Get_TransformCom()->Get_WorldMatrix();
	Matrix matWorld = m_pTransformCom->Get_WorldMatrix();

	matWorld = m_matOrigin * matBone * matPlayerWorld;

	m_pTransformCom->Set_WorldMatrix(matWorld);

	BillBoard();
}

void CRasenganCircle::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CRasenganCircle::Render()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(Vec4))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vBlurColor", &m_vBlurColor, sizeof(Vec4))))
		return E_FAIL;


	if (FAILED(m_pBaseTexture->Set_SRV(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;


	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();


	Vec4 vWhite = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vWhite, sizeof(Vec4))))
		return E_FAIL;
	
	m_pRingTexture->Set_SRV(m_pShaderCom, "g_MaskTexture");
	m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture2");
	
	
	m_pShaderCom->Begin(1);
	
	m_pVIBufferCom->Render();


	Safe_Release(pGameInstance);
	return S_OK;
}

void CRasenganCircle::Set_Owner(CPlayer_Naruto* pPlayer)
{
	m_pPlayer = pPlayer;
	m_iBoneIndex = m_pPlayer->Get_ModelCom()->Find_BoneIndex(L"R_Hand_Weapon_cnt_tr_end");
}


void CRasenganCircle::Appear()
{
	m_bActive = true;
	m_bEnd = false;

	m_matOrigin._11 = 0.001f;
	m_matOrigin._22 = 0.001f;


	Matrix matBone = m_pPlayer->Get_ModelCom()->Get_CurrBoneMatrix(m_iBoneIndex);
	Matrix matPlayerWorld = m_pPlayer->Get_TransformCom()->Get_WorldMatrix();
	Matrix matWorld = m_pTransformCom->Get_WorldMatrix();

	matWorld = m_matOrigin * matBone * matPlayerWorld;

	m_pTransformCom->Set_WorldMatrix(matWorld);

	BillBoard();

	m_pStartSmoke->Appear();
}

void CRasenganCircle::DisAppear()
{
	m_bEnd = true;
	m_pRunSmoke->DisAppear();
}

void CRasenganCircle::RunSmoke()
{
	m_pRunSmoke->Appear();
}

void CRasenganCircle::BillBoard()
{
	Vec3 vCamPos = CGameInstance::GetInstance()->Get_CamPosition();

	m_pTransformCom->LookAt(vCamPos);
}


HRESULT CRasenganCircle::Ready_Components()
{
	__super::Ready_Components();



	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_RasenganCircle"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Ring"),
		TEXT("Com_RingTexture"), (CComponent**)&m_pRingTexture)))
		return E_FAIL;


	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Circle"),
		TEXT("Com_BaseTexture"), (CComponent**)&m_pBaseTexture)))
		return E_FAIL;


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Tex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;




	return S_OK;
}


CGameObject* CRasenganCircle::Clone(void* pArg)
{
	CRasenganCircle* pInstance = new CRasenganCircle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CRasenganCircle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRasenganCircle* CRasenganCircle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRasenganCircle* pInstance = new CRasenganCircle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRasenganCircle");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CRasenganCircle::Free()
{
	__super::Free();
}
