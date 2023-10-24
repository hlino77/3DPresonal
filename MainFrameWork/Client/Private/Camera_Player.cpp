#include "stdafx.h"
#include "Camera_Player.h"
#include "GameInstance.h"
#include "Player.h"

CCamera_Player::CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, wstring strObjTag)
	: CCamera(pDevice, pContext, strObjTag)
{
}

CCamera_Player::CCamera_Player(const CCamera_Player& rhs, CTransform::TRANSFORMDESC* pArg)
	: CCamera(rhs, pArg)
{

}

HRESULT CCamera_Player::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Player::Initialize(void* pArg)
{
	PlayerCameraDesc* pDesc = static_cast<PlayerCameraDesc*>(pArg);
	m_pPlayer = pDesc->pPlayer;
	m_fSesitive = pDesc->fSensitive;
	m_fCameraSpeed = pDesc->fSpeed;

	if (FAILED(__super::Initialize(&pDesc->tCameraDesc)))
		return E_FAIL;

	m_vTargetOffset = Vec3(0.0f, 1.0f, 0.0f);

	m_fCameraLength = 4.0f;


	m_fCurrSpeedX = 0.0f;
	m_fCurrSpeedY = 0.0f;

	m_fSpeedX = 0.1f;
	m_fSpeedY = 0.1f;

	m_fMaxSpeedX = 0.03f;
	m_fMaxSpeedY = 0.03f;

	m_fBreak = 0.2f;
	
	m_pTransformCom->Set_State(CTransform::STATE::STATE_POSITION, pDesc->vInitPos);


	Vec3 vOffset = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	vOffset.Normalize();
	m_fCameraAngle = acosf(vOffset.Dot(Vec3(0.0f, 1.0f, 0.0f)));
	

	return S_OK;
}

void CCamera_Player::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	//오프셋을 매번 업데이트 한다
	// 속도로 회전 
	
	Matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	Matrix matPlayerWorld = m_pPlayer->Get_TransformCom()->Get_WorldMatrix();
	Vec3 vPlayerPos = Vec3(matPlayerWorld.m[3]) + m_vTargetOffset;

	Matrix matRotWorld = m_pTransformCom->Get_WorldMatrix();
	matRotWorld.Translation(vPlayerPos);
	Matrix matLocal = matWorld * matRotWorld.Invert();
	if (m_bMouseMove)
	{
		_long	MouseMove = 0;

		if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_Y))
		{
			_float fMoveDir = 0.0f;
			if (MouseMove < 0)
				fMoveDir = -1.0f;
			else
				fMoveDir = 1.0f;

			m_fCurrSpeedY += m_fSpeedY * fTimeDelta * fMoveDir;

			if (fabs(m_fCurrSpeedY) > m_fMaxSpeedY)
				m_fCurrSpeedY = (m_fCurrSpeedY / fabs(m_fCurrSpeedY)) * m_fMaxSpeedY;
		}
		else if (MouseMove == 0)
		{
			m_fCurrSpeedY -= m_fCurrSpeedY * m_fBreak;
			if (fabs(m_fCurrSpeedY) < 0.0001f)
				m_fCurrSpeedY = 0.0f;
		}


		if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_X))
		{
			_float fMoveDir = 0.0f;
			if (MouseMove < 0)
				fMoveDir = -1.0f;
			else
				fMoveDir = 1.0f;

			m_fCurrSpeedX += m_fSpeedX * fTimeDelta * fMoveDir;

			if (fabs(m_fCurrSpeedX) > m_fMaxSpeedX)
				m_fCurrSpeedX = (m_fCurrSpeedX / fabs(m_fCurrSpeedX)) * m_fMaxSpeedX;
		}
		else if (MouseMove == 0)
		{
			m_fCurrSpeedX -= m_fCurrSpeedX * m_fBreak;
			if (fabs(m_fCurrSpeedX) < 0.0001f)
				m_fCurrSpeedX = 0.0f;
		}



		if (m_fCurrSpeedY)
		{
			Vec3 vRight = Vec3(matLocal.m[0]);

			matLocal *= Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(vRight, m_fCurrSpeedY));
		}	
		if (m_fCurrSpeedX)
			matLocal *= Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vec3(0.0f, 1.0f, 0.0f), m_fCurrSpeedX));
	}
	matWorld = matLocal * matRotWorld;



	Vec3 vOffset = Vec3(matWorld.m[3]) - vPlayerPos;

	vOffset.Normalize();

	Vec3 vUp = Vec3(0.0f, 1.0f, 0.0f);

	if (m_fCurrSpeedY)
		m_fCameraAngle = acosf(vUp.Dot(vOffset));

	_float fAngle = XMConvertToDegrees(m_fCameraAngle);
	if (fAngle < 30.0f)
	{
		m_fCameraAngle = XMConvertToRadians(30.0f);
		m_fCurrSpeedY = 0.0f;
	}
	if (fAngle > 150.0f)
	{
		m_fCameraAngle = XMConvertToRadians(150.0f);
		m_fCurrSpeedY = 0.0f;
	}

	
	Vec3 vRight;
	vRight = Vec3(matWorld.m[3]) - vPlayerPos;
	vRight = vRight.Cross(vUp);
	vRight.Normalize();

	Vec3 vTargetPos = XMVector3Rotate(vUp, Quaternion::CreateFromAxisAngle(-vRight, m_fCameraAngle));
	vTargetPos *= m_fCameraLength;
	vTargetPos = vPlayerPos + vTargetPos;

	Vec3 vTargetLook = -vOffset;
	vTargetLook.Normalize();

	Vec3 vCamPos = Vec3(matWorld.m[3]);
	Vec3 vCamLook = Vec3(matWorld.m[2]);
	vCamLook.Normalize();

	
	_float fLerpSpeed = min(1.0f, m_fCameraSpeed * fTimeDelta);

	Vec3 vPos = Vec3::Lerp(vCamPos, vTargetPos, fLerpSpeed);

	Vec3 vLook = Vec3::Lerp(vCamLook, vTargetLook, fLerpSpeed);
	vLook.Normalize();


	matWorld = Matrix::CreateWorld(vPos, -vLook, Vec3(0.0f, 1.0f, 0.0f));


	m_pTransformCom->Set_WorldMatrix(matWorld);

	Safe_Release(pGameInstance);

	__super::Tick(fTimeDelta);
}

void CCamera_Player::LateTick(_float fTimeDelta)
{
}

HRESULT CCamera_Player::Render()
{
	return S_OK;
}

HRESULT CCamera_Player::Ready_Components()
{
	__super::Ready_Components();
	return S_OK;
}

CCamera_Player* CCamera_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, wstring strObjTag)
{
	CCamera_Player* pInstance = new CCamera_Player(pDevice, pContext, strObjTag);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Player::Clone(void* pArg)
{
	CCamera::CAMERADESC* pCameraDesc = (CCamera::CAMERADESC*)pArg;

	CCamera_Player* pInstance = new CCamera_Player(*this, &pCameraDesc->TransformDesc);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Created : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Player::Free()
{
	__super::Free();


}
