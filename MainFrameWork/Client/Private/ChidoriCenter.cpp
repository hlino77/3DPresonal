#include "stdafx.h"
#include "ChidoriCenter.h"
#include "Client_Defines.h"
#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
#include "VIBuffer_Point.h"
#include "RigidBody.h"
#include "Pool.h"
#include "Player_Sasuke.h"

CChidoriCenter::CChidoriCenter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CChidoriCenter::CChidoriCenter(const CChidoriCenter& rhs)
	: CEffect(rhs)
{
}

HRESULT CChidoriCenter::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CChidoriCenter::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	m_szModelName = L"Effect_ChidoriCenter";

	m_bActive = false;


	m_vColor = Vec4(0.81f, 0.96f, 0.95f, 1.0f);
	m_vBlurColor = Vec4(0.5f, 0.84f, 1.0f, 1.0f);

	m_vBlurColor *= 0.2f;

	m_vScale = Vec3(0.5f, 0.5f, 0.5f);



	m_matOrigin = m_pTransformCom->Get_WorldMatrix();


	m_vUVMaxCount = Vec2(2.0f, 2.0f);


	m_fUVTime = 0.05f;
	m_fCurrUVTime = 0.0f;

	return S_OK;
}

void CChidoriCenter::Tick(_float fTimeDelta)
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
			m_matOrigin._11 -= 2.0f * fTimeDelta;

			if (m_matOrigin._11 < 0.001f)
				m_matOrigin._11 = 0.001f;
		}

		if (m_matOrigin._22 > 0.001f)
		{
			m_matOrigin._22 -= 2.0f * fTimeDelta;

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
			m_matOrigin._11 += 2.0f * fTimeDelta;

			if (m_matOrigin._11 > m_vScale.x)
				m_matOrigin._11 = m_vScale.x;
		}

		if (m_matOrigin._22 < m_vScale.y)
		{
			m_matOrigin._22 += 2.0f * fTimeDelta;

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

	Update_UV(fTimeDelta);
}

void CChidoriCenter::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CChidoriCenter::Render()
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


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVIndex", &m_vUVIndex, sizeof(Vec2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaxCount", &m_vUVMaxCount, sizeof(Vec2))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;





	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render();


	Safe_Release(pGameInstance);
	return S_OK;
}

void CChidoriCenter::Set_Owner(CPlayer_Sasuke* pPlayer)
{
	m_pPlayer = pPlayer;
	m_iBoneIndex = m_pPlayer->Get_ModelCom()->Find_BoneIndex(L"LeftHandThumb3_end");
}


void CChidoriCenter::Appear()
{
	m_bActive = true;
	m_bEnd = false;

	m_matOrigin._11 = 0.01f;
	m_matOrigin._22 = 0.01f;


	Matrix matBone = m_pPlayer->Get_ModelCom()->Get_CurrBoneMatrix(m_iBoneIndex);
	Matrix matPlayerWorld = m_pPlayer->Get_TransformCom()->Get_WorldMatrix();
	Matrix matWorld = m_pTransformCom->Get_WorldMatrix();

	matWorld = m_matOrigin * matBone * matPlayerWorld;

	m_pTransformCom->Set_WorldMatrix(matWorld);

	BillBoard();


	m_vUVIndex = Vec2(0.0f, 0.0f);

}

void CChidoriCenter::DisAppear()
{
	m_bEnd = true;
}

void CChidoriCenter::BillBoard()
{
	Vec3 vCamPos = CGameInstance::GetInstance()->Get_CamPosition();

	m_pTransformCom->LookAt(vCamPos);
}


HRESULT CChidoriCenter::Ready_Components()
{
	__super::Ready_Components();



	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_ChidoriCenter"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Tex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;




	return S_OK;
}

void CChidoriCenter::Update_UV(_float fTimeDelta)
{
	m_fCurrUVTime += fTimeDelta;

	if (m_fCurrUVTime >= m_fUVTime)
	{
		m_vUVIndex.x += 1.0f;

		if (m_vUVIndex.x >= m_vUVMaxCount.x)
		{
			m_vUVIndex.x = 0.0f;

			m_vUVIndex.y += 1.0f;
			if (m_vUVIndex.y >= m_vUVMaxCount.y)
				m_vUVIndex.y = 0.0f;
		}

		m_fCurrUVTime = 0.0f;
	}
}


CGameObject* CChidoriCenter::Clone(void* pArg)
{
	CChidoriCenter* pInstance = new CChidoriCenter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CLineCircle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CChidoriCenter* CChidoriCenter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CChidoriCenter* pInstance = new CChidoriCenter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLineCircle");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CChidoriCenter::Free()
{
	__super::Free();
}
