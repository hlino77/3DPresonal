#include "stdafx.h"
#include "LineCircle.h"
#include "Client_Defines.h"
#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
#include "VIBuffer_Point.h"
#include "RigidBody.h"
#include "Pool.h"

CLineCircle::CLineCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CLineCircle::CLineCircle(const CLineCircle& rhs)
	: CEffect(rhs)
{
}

HRESULT CLineCircle::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CLineCircle::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	m_pRigidBody->UseGravity(true);
	m_pRigidBody->SetCompareGruond(false);
	m_pRigidBody->SetMass(0.07f);

	m_szModelName = L"Effect_LineCircle";

	m_bActive = false;


	m_RandomNumber = mt19937_64((*m_RandomDevice)());

	//uniform_real_distribution


	m_RandomScaleX = uniform_real_distribution<float>(0.01f, 0.03f);
	m_RandomScaleY = uniform_real_distribution<float>(0.5f, 1.0f);
	m_RandomForce = uniform_real_distribution<float>(0.5f, 1.5f);
	m_RandomAngle = uniform_real_distribution<float>(0.0f, 360.0f);


	m_RandomDir = uniform_real_distribution<float>(-1.0f, 1.0f);
	m_RandomLifeTime = uniform_real_distribution<float>(0.5f, 1.5f);

	m_pRendererCom->Reserve_RenderGroup(CRenderer::RENDER_EFFECT_INSTANCE, this);

	return S_OK;
}

void CLineCircle::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bEnd)
	{
		Vec3 vVelocity = m_pRigidBody->GetLinearVelocity();

		vVelocity *= 1.0f - (5.f * fTimeDelta);

		m_pRigidBody->SetLinearVelocity(vVelocity);


		if (vVelocity.Length() <= 0.1f)
		{
			Set_Active(false);
			CPool<CLineCircle>::Return_Obj(this);
		}
	}


	if (m_fLifeTime <= 0.0f)
	{
		m_bEnd = true;
		m_pRigidBody->Set_Gravity(false);
	}
	else
		m_fLifeTime -= fTimeDelta;



	Vec3 vVelocity = m_pRigidBody->GetLinearVelocity();
	
	if (vVelocity.Length() <= 1.0f)
		return;

	vVelocity *= 1.0f - (10.f * fTimeDelta);

	m_pRigidBody->SetLinearVelocity(vVelocity);


}

void CLineCircle::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT_INSTANCE, this);
}

HRESULT CLineCircle::Render()
{
	return S_OK;
}

HRESULT CLineCircle::Render_Instance(ID3D11Buffer* pInstanceBuffer, _uint iSize)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &Vec3(pGameInstance->Get_CamPosition()), sizeof(Vec3))))
		return E_FAIL;


	Vec4 vEmissive(1.0f, 1.0f, 1.0f, 0.0f);

	vEmissive *= 0.8f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vEmissive", &vEmissive, sizeof(Vec4))))
		return E_FAIL;

	m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture");

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render_Instance(pInstanceBuffer, iSize, sizeof(VTXINSTANCE_POINTEFFECT));


	Safe_Release(pGameInstance);
	return S_OK;
}

void CLineCircle::Add_InstanceData(vector<Vec4>& BufferData)
{
	Vec4 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vec4 vDir = m_pRigidBody->GetLinearVelocity();
	vDir *= 0.07f;

	Vec4 vScale = m_vScale;

	BufferData.push_back(vPosition);
	BufferData.push_back(vDir);
	BufferData.push_back(vScale);
	BufferData.push_back(m_vColor);
	BufferData.push_back(m_vBlurColor);
}

void CLineCircle::Appear(Vec3 vPos, Vec4 vColor, Vec4 vBlurColor, _float fForce)
{
	m_bActive = true;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_vColor = vColor;
	m_vBlurColor = vBlurColor;


	_float fAngle = m_RandomAngle(m_RandomNumber);
	Vec3 vDir;
	vDir.x = m_RandomDir(m_RandomNumber);
	vDir.y = m_RandomDir(m_RandomNumber);
	vDir.z = m_RandomDir(m_RandomNumber);
	vDir.Normalize();

	_float fRandomForce = m_RandomForce(m_RandomNumber);

	fRandomForce *= fForce;
	m_pRigidBody->AddForce(vDir * fRandomForce, ForceMode::FORCE);

	//0.05f 0.01f 1.0f, 0.5f;

	m_vScale.x = m_RandomScaleX(m_RandomNumber);
	m_vScale.y = m_RandomScaleY(m_RandomNumber);


	m_fLifeTime = m_RandomLifeTime(m_RandomNumber);

	m_bEnd = false;

	m_pRigidBody->Set_Gravity(true);
}

void CLineCircle::Appear_OnlyUp(Vec3 vPos, Vec4 vColor, Vec4 vBlurColor, _float fForce)
{
	m_bActive = true;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_vColor = vColor;
	m_vBlurColor = vBlurColor;


	_float fAngle = m_RandomAngle(m_RandomNumber);
	Vec3 vDir;
	vDir.x = m_RandomDir(m_RandomNumber);
	vDir.y = m_RandomDir(m_RandomNumber);
	vDir.z = m_RandomDir(m_RandomNumber);
	vDir.Normalize();

	if (vDir.y < 0.0f)
		vDir.y *= -1.0f;

	_float fRandomForce = m_RandomForce(m_RandomNumber);

	fRandomForce *= fForce;
	m_pRigidBody->AddForce(vDir * fRandomForce, ForceMode::FORCE);

	//0.05f 0.01f 1.0f, 0.5f;

	m_vScale.x = m_RandomScaleX(m_RandomNumber);
	m_vScale.y = m_RandomScaleY(m_RandomNumber);


	m_fLifeTime = m_RandomLifeTime(m_RandomNumber);

	m_bEnd = false;

	m_pRigidBody->Set_Gravity(true);
}


HRESULT CLineCircle::Ready_Components()
{
	__super::Ready_Components();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Point_Instance"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Circle"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}


CGameObject* CLineCircle::Clone(void* pArg)
{
	CLineCircle* pInstance = new CLineCircle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CLineCircle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CLineCircle* CLineCircle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLineCircle* pInstance = new CLineCircle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLineCircle");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CLineCircle::Free()
{
	__super::Free();
}
