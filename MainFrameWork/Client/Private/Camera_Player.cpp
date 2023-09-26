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

	m_vOffset = Vec3(0.0f, 1.0f, 0.0f);


	m_fCameraLength = 4.0f;


	m_fCurrSpeedX = 0.0f;
	m_fCurrSpeedY = 0.0f;

	m_fSpeedX = 0.01f;
	m_fSpeedY = 0.005f;

	m_fMaxSpeedX = 0.04f;
	m_fMaxSpeedY = 0.02f;

	m_fBreak = 0.1f;

	return S_OK;
}

void CCamera_Player::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	//오프셋을 매번 업데이트 한다
	// 속도로 회전 
	
	Matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	Matrix matLocal = matWorld * m_pPlayer->Get_TransformCom()->Get_WorldMatrixInverse();
	Vec3 vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE::STATE_POSITION) + m_vOffset;

	if (m_bMouseMove)
	{
		/*POINT MousePos;

		MousePos.x = g_iWinSizeX / 2;
		MousePos.y = g_iWinSizeY / 2;

		ClientToScreen(g_hWnd, &MousePos);
		SetCursorPos(MousePos.x, MousePos.y);*/

		_long	MouseMove = 0;

		if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_Y))
		{
			m_fCurrSpeedY += MouseMove * fTimeDelta * m_fSpeedY;

			if (fabs(m_fCurrSpeedY) > m_fMaxSpeedY)
				m_fCurrSpeedY = (m_fCurrSpeedY / fabs(m_fCurrSpeedY)) * m_fMaxSpeedY;
		}
		else if (MouseMove == 0)
		{
			m_fCurrSpeedY -= m_fCurrSpeedY * m_fBreak;
			if (fabs(m_fCurrSpeedY) < 0.001f)
				m_fCurrSpeedY = 0.0f;
		}
			

		if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_X))
		{
			m_fCurrSpeedX += MouseMove * fTimeDelta * m_fSpeedX;

			if (fabs(m_fCurrSpeedX) > m_fMaxSpeedX)
				m_fCurrSpeedX = (m_fCurrSpeedX / fabs(m_fCurrSpeedX)) * m_fMaxSpeedX;
		}
		else if (MouseMove == 0)
		{
			m_fCurrSpeedX -= m_fCurrSpeedX * m_fBreak;
			if (fabs(m_fCurrSpeedX) < 0.001f)
				m_fCurrSpeedX = 0.0f;
		}


		if(m_fCurrSpeedY)
			matLocal *= Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vec3(matLocal.m[0]), m_fCurrSpeedY));
		if(m_fCurrSpeedX)
			matLocal *= Matrix::CreateFromQuaternion(Quaternion::CreateFromAxisAngle(Vec3(0.0f, 1.0f, 0.0f), m_fCurrSpeedX));
	}
	matLocal *= m_pPlayer->Get_TransformCom()->Get_WorldMatrix();


	Vec3 vCamPos(matLocal.m[3]);
	
	Vec3 vTargetLook = vPlayerPos - vCamPos;
	vTargetLook.Normalize();
	Vec3 vTargetPos = vPlayerPos + (vTargetLook * -m_fCameraLength);
	vTargetPos.y = matLocal.m[3][1];

	Vec3 vPos = Vec3::Lerp(vCamPos, vTargetPos, m_fCameraSpeed);

	Vec3 vLook = Vec3::Lerp(Vec3(matLocal.m[2]), vTargetLook, m_fCameraSpeed);
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
