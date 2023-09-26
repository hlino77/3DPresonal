#include "..\Public\Transform.h"


CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{

}

void CTransform::Set_State(STATE eState, Vec3 vState)
{
	WRITE_LOCK
	m_WorldMatrix.m[eState][0] = vState.x;
	m_WorldMatrix.m[eState][1] = vState.y;
	m_WorldMatrix.m[eState][2] = vState.z;
}

HRESULT CTransform::Initialize_Prototype()
{
	/* vector -> float : XMStore*/
	/* float -> vector : XMLoad */

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());	

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	return S_OK;
}

void CTransform::Go_Straight(_float fSpeed, _float fTimeDelta)
{
	Vec3 vPosition = Get_State(CTransform::STATE_POSITION);
	Vec3 vLook = Get_State(CTransform::STATE_LOOK);
	vLook.Normalize();

	vPosition += vLook * fSpeed * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fSpeed, _float fTimeDelta)
{
	Vec3 vPosition = Get_State(CTransform::STATE_POSITION);
	Vec3 vLook = Get_State(CTransform::STATE_LOOK);
	vLook.Normalize();

	vPosition -= vLook * fSpeed * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fSpeed, _float fTimeDelta)
{
	Vec3 vPosition = Get_State(CTransform::STATE_POSITION);
	Vec3 vLook = Get_State(CTransform::STATE_RIGHT);
	vLook.Normalize();

	vPosition -= vLook * fSpeed * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fSpeed, _float fTimeDelta)
{
	Vec3 vPosition = Get_State(CTransform::STATE_POSITION);
	Vec3 vLook = Get_State(CTransform::STATE_RIGHT);
	vLook.Normalize();

	vPosition += vLook * fSpeed * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Up(_float fTimeDelta)
{
	Vec3 vPosition = Get_State(CTransform::STATE_POSITION);
	Vec3 vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	vUp.Normalize();

	vPosition += vUp * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Down(_float fTimeDelta)
{
	Vec3 vPosition = Get_State(CTransform::STATE_POSITION);
	Vec3 vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	vUp.Normalize();

	vPosition -= vUp * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Set_Scale(Vec3 vScaleInfo)
{
	Vec3 vRight = Get_State(CTransform::STATE_RIGHT);
	vRight.Normalize();

	Vec3 vUp = Get_State(CTransform::STATE_UP);
	vUp.Normalize();

	Vec3 vLook = Get_State(CTransform::STATE_LOOK);
	vLook.Normalize();


	WRITE_LOCK
	Set_State(CTransform::STATE_RIGHT, vRight * vScaleInfo.x);
	Set_State(CTransform::STATE_UP, vUp * vScaleInfo.y);
	Set_State(CTransform::STATE_LOOK, vLook * vScaleInfo.z);
}

Vec3 CTransform::Get_Scale()
{
	READ_LOCK
	return Vec3(Get_State(CTransform::STATE_RIGHT).Length(), Get_State(CTransform::STATE_UP).Length(), Get_State(CTransform::STATE_LOOK).Length());
}

void CTransform::Turn(Vec3 vAxis, _float fTimeDelta)
{
	Quaternion vRot = Quaternion::CreateFromAxisAngle(vAxis, m_TransformDesc.fRotationPerSec * fTimeDelta);
	Matrix matRot = Matrix::CreateFromQuaternion(vRot);
	
	Matrix matWorld = m_WorldMatrix * matRot;


	WRITE_LOCK
	Set_State(CTransform::STATE_RIGHT, Vec3(matWorld.m[0]));
	Set_State(CTransform::STATE_UP, Vec3(matWorld.m[1]));
	Set_State(CTransform::STATE_LOOK, Vec3(matWorld.m[2]));
}

void CTransform::Rotation(Vec3 vAxis, _float fRadian)
{
	// Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(60.0f));

	Matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	RotationMatrix.CreateFromQuaternion(Quaternion::CreateFromAxisAngle(vAxis, fRadian));
	Vec3 vScale = Get_Scale();

	Vec3 vRight(RotationMatrix.m[0]);
	Vec3 vUp(RotationMatrix.m[1]);
	Vec3 vLook(RotationMatrix.m[2]);

	vRight.Normalize();
	vUp.Normalize();
	vLook.Normalize();


	WRITE_LOCK
	Set_State(CTransform::STATE_RIGHT, vRight  * vScale.x);
	Set_State(CTransform::STATE_RIGHT, vUp * vScale.y);
	Set_State(CTransform::STATE_RIGHT, vLook * vScale.z);
}

void CTransform::LookAt(Vec3 vAt)
{
	Vec3		vLook = vAt - Get_State(CTransform::STATE_POSITION);

	Vec3		vRight = Vec3(0.0f, 1.0f, 0.0f).Cross(vLook);

	Vec3		vUp = vLook.Cross(vRight);

	Vec3		vScale = Get_Scale();


	WRITE_LOCK
	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookAt_ForLandObject(Vec3 vAt)
{
	Vec3 vLook = vAt - Get_State(CTransform::STATE_POSITION);

	Vec3 vScale = Get_Scale();


	Vec3 vRight = Vec3(0.0f, 1.0f, 0.0f).Cross(vLook);
	vRight.Normalize();
	vRight *= vScale.x;

	vLook = vRight.Cross(Get_State(CTransform::STATE_UP));
	vLook.Normalize();
	vLook *= vScale.z;


	WRITE_LOCK
	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_LOOK, vLook);
}

void CTransform::Move(Vec3 vTargetPos, _float fTimeDelta, _float fLimitDistance)
{
	Vec3		vPosition = Get_State(CTransform::STATE_POSITION);
	Vec3		vDirection = vTargetPos - vPosition;

	_float		fDistance = vDirection.Length();

	vDirection.Normalize();

	if(fDistance > fLimitDistance)
		vPosition += vDirection * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*			pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*			pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CTransform::Free()
{
	__super::Free();
}

void CTransform::Move_Dir(Vec3 vDir, _float fTimeDelta)
{
	vDir.Normalize();
	Vec3 vPosition = Get_State(CTransform::STATE_POSITION);
	vPosition += vDir * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::LookAt_Lerp(Vec3 vAt, _float fSpeed, _float fTimeDelta)
{
	Vec3 vPlayerLook = Get_State(CTransform::STATE_LOOK);
	vPlayerLook.Normalize();

	Vec3 vTargetLook = vAt;
	vTargetLook.Normalize();

	Vec3 vUp = Get_State(CTransform::STATE_UP);
	vUp.Normalize();


	_float fRadian = acosf(min(1.0f,vTargetLook.Dot(vPlayerLook)));
	if (fRadian <= fSpeed * fTimeDelta)
	{
		vPlayerLook = vTargetLook;

		Vec3 vRight = vUp.Cross(vPlayerLook);
		vRight.Normalize();

		vPlayerLook = vRight.Cross(vUp);
		vPlayerLook.Normalize();

		vUp = vPlayerLook.Cross(vRight);
		vUp.Normalize();

		Vec3 vScale = Get_Scale();

		vRight *= vScale.x;
		vUp *= vScale.y;
		vPlayerLook *= vScale.z;

		WRITE_LOCK
		Set_State(STATE::STATE_RIGHT, vRight);
		Set_State(STATE::STATE_UP, vUp);
		Set_State(STATE::STATE_LOOK, vPlayerLook);

		return;
	}
	
	Vec3 vPlayerUp = Get_State(CTransform::STATE_UP);
	Vec3 vCrossUp = vPlayerLook.Cross(vTargetLook);

	if (vPlayerUp.Dot(vCrossUp) >= 0)
	{
		Turn_Speed(vUp, fSpeed, fTimeDelta);
	}
	else
	{
		Turn_Speed(vUp, -fSpeed, fTimeDelta);
	}
}

void CTransform::Turn_Speed(Vec3 vAxis, _float fSpeed, _float fTimeDelta)
{
	Quaternion vRot = Quaternion::CreateFromAxisAngle(vAxis, fSpeed * fTimeDelta);
	Matrix matRot = Matrix::CreateFromQuaternion(vRot);

	Matrix matWorld = m_WorldMatrix * matRot;


	WRITE_LOCK
	Set_State(CTransform::STATE_RIGHT, Vec3(matWorld.m[0]));
	Set_State(CTransform::STATE_UP, Vec3(matWorld.m[1]));
	Set_State(CTransform::STATE_LOOK, Vec3(matWorld.m[2]));
}
