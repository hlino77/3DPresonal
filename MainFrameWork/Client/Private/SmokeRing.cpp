#include "stdafx.h"
#include "SmokeRing.h"
#include "Client_Defines.h"
#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
#include "VIBuffer_Point.h"
#include "RigidBody.h"
#include "Pool.h"
#include "Player_Naruto.h"

CSmokeRing::CSmokeRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CSmokeRing::CSmokeRing(const CSmokeRing& rhs)
	: CEffect(rhs)
{
}

HRESULT CSmokeRing::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CSmokeRing::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	m_szModelName = L"Effect_SmokeRing";

	m_bActive = false;


	m_vColor = Vec4(0.5f, 0.84f, 1.0f, 1.0f);
	//m_vBlur = Vec4(0.5f, 0.84f, 1.0f, 1.0f);

	//m_vBlur = Vec4(0.07f, 0.3f, 0.63f, 1.0f);


	m_vBlur = m_vColor * 0.3f;


	return S_OK;
}

void CSmokeRing::Tick(_float fTimeDelta)
{
	Vec3 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	vRight.Normalize();
	m_pTransformCom->Turn_Speed(vRight, 20.0f, fTimeDelta);

	



	Vec3 vScale = m_pTransformCom->Get_Scale();


	if (vScale != m_vMaxScale)
	{
		vScale = Vec3::Lerp(vScale, m_vMaxScale, 10.0f * fTimeDelta);
		m_pTransformCom->Set_Scale(vScale);
	}


	if (m_bEnd)
	{
		m_vColor.w -= 2.0f * fTimeDelta;

		if (m_vColor.w <= 0.0f)
		{
			Set_Active(false);
			CPool<CSmokeRing>::Return_Obj(this);
		}

	}
	else
	{
		m_fCurrTime += fTimeDelta;

		if (m_fCurrTime >= m_fEffectTime)
		{
			m_bEnd = true;
		}
	}

}

void CSmokeRing::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODELEFFECT_INSTANCE, this);
}

HRESULT CSmokeRing::Render()
{
	
	return S_OK;
}

HRESULT CSmokeRing::Render_Instance(ID3D11Buffer* pInstanceBuffer, _uint iSize)
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return S_OK;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return S_OK;


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_uint iStride = sizeof(VTXINSTANCE_MODELEFFECT);


	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return S_OK;

		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/


		if (FAILED(m_pModelCom->Render_Instance(pInstanceBuffer, iSize, m_pShaderCom, i, iStride, 0)))
			return S_OK;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

void CSmokeRing::Add_InstanceData(vector<Vec4>& BufferData)
{
	Matrix matWorld = m_pTransformCom->Get_WorldMatrix();




	BufferData.push_back(Vec4(matWorld.m[0]));
	BufferData.push_back(Vec4(matWorld.m[1]));
	BufferData.push_back(Vec4(matWorld.m[2]));
	BufferData.push_back(Vec4(matWorld.m[3]));
	BufferData.push_back(m_vColor);
	BufferData.push_back(m_vBlur);
}


void CSmokeRing::Appear(Vec3 vPos, _float fBaseScale, Vec4 vColor, Vec4 vBlurColor, _float fTime)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_bActive = true;


	Matrix matWorld = XMMatrixIdentity();

	m_vMaxScale = Vec3(0.3f, 1.0f, 1.0f);

	_float fScale = fBaseScale + ((rand() % 10 + 1) * 0.1f);

	m_vMaxScale *= fScale;

	matWorld._11 = 0.03f;
	matWorld._22 = 0.1f;
	matWorld._33 = 0.1f;


	_float fYaw, fPitch, fRoll;

	fYaw = XMConvertToRadians(rand() % 360);
	fRoll = XMConvertToRadians(rand() % 360);
	fPitch = XMConvertToRadians(rand() % 360);

	Matrix matRot = Matrix::CreateFromYawPitchRoll(fYaw, fPitch, fRoll);


	matWorld *= matRot;
	matWorld.Translation(vPos);

	m_pTransformCom->Set_WorldMatrix(matWorld);


	m_vColor = vColor;
	m_vBlur = vBlurColor;


	m_fEffectTime = fTime;
	m_fCurrTime = 0.0f;

	m_bEnd = false;
}

void CSmokeRing::DisAppear()
{
	m_bEnd = true;
}



HRESULT CSmokeRing::Ready_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_LockFree_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ModelEffect_Instance"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_SmokeRing"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	///* For.Com_Model */
	wstring strComName = L"Prototype_Component_Model_SmokeRing";
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strComName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	Safe_Release(pGameInstance);



	return S_OK;
}


CGameObject* CSmokeRing::Clone(void* pArg)
{
	CSmokeRing* pInstance = new CSmokeRing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CRing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSmokeRing* CSmokeRing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSmokeRing* pInstance = new CSmokeRing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRing");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CSmokeRing::Free()
{
	__super::Free();
}
