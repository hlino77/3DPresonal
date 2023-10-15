#include "RigidBody.h"
#include "GameObject.h"
#include "Transform.h"

CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_IsSleeping(false)
	, m_fSleepThreshold(0.05f)
	, m_UseGravity(true)
	, m_vGravityDir(Vec3(0.0f, -1.0f, 0.0f))
	, m_IsKinematic(false)
	, m_fMass(1.f)
	, m_fDrag(0.01f)
	, m_fGroundDrag(0.05f)
	, m_fAngularDrag(0.01f)
	, m_byConstraints(0)
	, m_vLinearAcceleration(Vec3::Zero)
	, m_vAngularAcceleration(Vec3::Zero)
	, m_vLinearVelocity(Vec3::Zero)
	, m_vAngularVelocity(Vec3::Zero)
{
}

CRigidBody::CRigidBody(const CRigidBody& rhs)
	: CComponent(rhs)
	, m_IsSleeping(rhs.m_IsSleeping)
	, m_fSleepThreshold(rhs.m_fSleepThreshold)
	, m_UseGravity(rhs.m_UseGravity)
	, m_vGravityDir(rhs.m_vGravityDir)
	, m_IsKinematic(rhs.m_IsKinematic)
	, m_fMass(rhs.m_fMass)
	, m_fDrag(rhs.m_fDrag)
	, m_fGroundDrag(rhs.m_fGroundDrag)
	, m_fAngularDrag(rhs.m_fAngularDrag)
	, m_byConstraints(rhs.m_byConstraints)
	, m_vLinearAcceleration(rhs.m_vLinearAcceleration)
	, m_vAngularAcceleration(rhs.m_vAngularAcceleration)
	, m_vLinearVelocity(rhs.m_vLinearVelocity)
	, m_vAngularVelocity(rhs.m_vAngularVelocity)
{
}

HRESULT CRigidBody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRigidBody::Initialize(void* pArg)
{

	return S_OK;
}

void CRigidBody::Tick(const _float& fTimeDelta)	// FixedUpdate ó�� �����ϱ� ���� RigidBody�� ������Ʈ�� ���� �켱 ȣ���ؾ� ��.
{
	if (m_IsSleeping)
		return;

	//if (m_fSleepThreshold > m_vLinearVelocity.Length() && m_fSleepThreshold > 180.f * XM_1DIVPI * m_vAngularVelocity.Length())
	//{
	//	Sleep();
	//	return;
	//}

	if (m_IsKinematic)	// Kinematic
		KinematicUpdate(fTimeDelta);
	else				// Kinetic
		KineticUpdate(fTimeDelta);
}

void CRigidBody::LateTick(const _float& fTimeDelta)
{
}

void CRigidBody::DebugRender()
{
}

void CRigidBody::UseGravity(_bool UseGravity)
{
	if (UseGravity)
	{
		m_UseGravity = true;
		m_bGround = false;
	}
	else
	{
		m_UseGravity = false;
		m_bGround = true;
		if (m_vLinearVelocity.Length() > 0.0f)
		{
			Vec3 vGravityVel = m_vGravityDir * m_vLinearVelocity.Dot(m_vGravityDir);
			m_vLinearVelocity -= vGravityVel;
		}
	}
}

void CRigidBody::KineticUpdate(const _float& fTimeDelta)
{
	if (m_UseGravity)
		m_vLinearVelocity += m_vGravityDir * 9.81f * fTimeDelta * m_fMass;

	m_vAngularVelocity += m_vAngularAcceleration;
	m_vLinearVelocity += m_vLinearAcceleration;

	_float fAngularResistance = m_fAngularDrag;
	_float fLinearResistance = m_fDrag;
	_float fLinearGroundResistance = m_fGroundDrag;

	//(fAngularResistance < 1.f) ? (m_vAngularVelocity = m_vAngularVelocity * (1.f - fAngularResistance)) : (m_vAngularVelocity = Vec3::Zero);
	//(fLinearResistance < 1.f) ? (m_vLinearVelocity = m_vLinearVelocity * (1.f - fLinearResistance)) : (m_vLinearVelocity = Vec3::Zero);


	if (m_bGround)
		(fLinearGroundResistance < 1.f) ? (m_vLinearVelocity = m_vLinearVelocity * (1.f - fLinearGroundResistance)) : (m_vLinearVelocity = Vec3::Zero);

	// Constraints Check
	if (m_byConstraints)
	{
		_int i = 0;
		while(i < 3)
			(m_byConstraints & 1 << i) ? (*(((_float*)&m_vLinearVelocity) + i++) = 0) : i++;
		while(i < 6)
			(m_byConstraints & 1 << i) ? (*(((_float*)&m_vAngularVelocity) + i++) = 0) : i++;
	}

	UpdateTransform(fTimeDelta);

	m_vAngularAcceleration = Vec3::Zero;
	m_vLinearAcceleration = Vec3::Zero;




	UpdateGround(fTimeDelta);

}

void CRigidBody::KinematicUpdate(const _float& fTimeDelta)
{
	UpdateTransform(fTimeDelta);

	//ClearNetPower();
	ClearForce(ForceMode::VELOCITY_CHANGE);
	ClearTorque(ForceMode::VELOCITY_CHANGE);
}

void CRigidBody::UpdateTransform(const _float& fTimeDelta)
{
	CTransform* pTransform = m_pOwner->Get_TransformCom();

	//pTransform->Rotate(m_vAngularVelocity * fTimeDelta);
	pTransform->Move_Pos(m_vLinearVelocity * fTimeDelta);
}

void CRigidBody::UpdateGround(const _float& fTimeDelta)
{
	CTransform* pTransform = m_pOwner->Get_TransformCom();

	Vec3 vPos = pTransform->Get_State(CTransform::STATE_POSITION);
	
	if (vPos.y < 0.0f)
	{
		vPos.y = 0.f;
		pTransform->Set_State(CTransform::STATE_POSITION, vPos);
		UseGravity(false);
	}
}

void CRigidBody::AddForce(const Vec3& vForce, ForceMode eMode)
{
	switch (eMode)
	{
	case ForceMode::FORCE:
		m_vLinearAcceleration += vForce / m_fMass;
		break;
	case ForceMode::IMPULSE:
		m_vLinearVelocity += vForce / m_fMass;
		break;
	case ForceMode::ACCELERATION:
		m_vLinearAcceleration += vForce;
		break;
	case ForceMode::VELOCITY_CHANGE:
		m_vLinearVelocity += vForce;
		break;
	}

	WakeUp();
}

void CRigidBody::AddTorque(const Vec3& vTorque, ForceMode eMode)
{
	switch (eMode)
	{
	case ForceMode::FORCE:								// ���� : r == 1	 �̸� T == F
		m_vAngularAcceleration += vTorque / m_fMass;	// (�� ����)���� ���Ʈ I == m * pow(r, 2) // �����ӵ� a == T / I
		break;
	case ForceMode::IMPULSE:
		m_vAngularVelocity += vTorque / m_fMass;		// ����� L == I * w	// ������ I == m // ���� ���ӵ� w == L / m
		break;
	case ForceMode::ACCELERATION:
		m_vAngularAcceleration += vTorque;
		break;
	case ForceMode::VELOCITY_CHANGE:
		m_vAngularVelocity += vTorque;
		break;
	}

	WakeUp();
}

void CRigidBody::SetForceAndTorque(const Vec3& vForce, const Vec3& vTorque, ForceMode eMode)
{
	switch (eMode)
	{
	case ForceMode::FORCE:
		m_vLinearAcceleration = vForce / m_fMass;
		m_vAngularAcceleration = vTorque / m_fMass;
		break;
	case ForceMode::IMPULSE:
		m_vLinearVelocity = vForce / m_fMass;
		m_vAngularVelocity = vTorque / m_fMass;
		break;
	case ForceMode::ACCELERATION:
		m_vLinearAcceleration = vForce;
		m_vAngularAcceleration = vTorque;
		break;
	case ForceMode::VELOCITY_CHANGE:
		m_vLinearVelocity = vForce;
		m_vAngularVelocity = vTorque;
		break;
	}

	WakeUp();
}

void CRigidBody::ClearForce(ForceMode eMode)
{
	switch (eMode)
	{
	case ForceMode::FORCE:
		::ZeroMemory(&m_vLinearAcceleration, sizeof(Vec3));
		break;
	case ForceMode::IMPULSE:
		::ZeroMemory(&m_vLinearVelocity, sizeof(Vec3));
		break;
	case ForceMode::ACCELERATION:
		::ZeroMemory(&m_vLinearAcceleration, sizeof(Vec3));
		break;
	case ForceMode::VELOCITY_CHANGE:
		::ZeroMemory(&m_vLinearVelocity, sizeof(Vec3));
		break;
	}
}

void CRigidBody::ClearTorque(ForceMode eMode)
{
	switch (eMode)
	{
	case ForceMode::FORCE:										// ���� : r == 1	 �̸� T == F
		::ZeroMemory(&m_vAngularAcceleration, sizeof(Vec3));	// (�� ����)���� ���Ʈ I == m * pow(r, 2) // �����ӵ� a == T / I
		break;
	case ForceMode::IMPULSE:
		::ZeroMemory(&m_vAngularVelocity, sizeof(Vec3));		// ����� L == I * w	// ������ I == m // ���� ���ӵ� w == L / m
		break;
	case ForceMode::ACCELERATION:
		::ZeroMemory(&m_vAngularAcceleration, sizeof(Vec3));
		break;
	case ForceMode::VELOCITY_CHANGE:
		::ZeroMemory(&m_vAngularVelocity, sizeof(Vec3));
		break;
	}
}

void CRigidBody::ClearNetPower()
{
	::ZeroMemory(&m_vLinearAcceleration, sizeof(Vec3));
	::ZeroMemory(&m_vAngularAcceleration, sizeof(Vec3));
	::ZeroMemory(&m_vAngularVelocity, sizeof(Vec3));
	::ZeroMemory(&m_vLinearVelocity, sizeof(Vec3));
}

void CRigidBody::SetLinearVelocity(const Vec3& vLinearVelocity)
{
	m_vLinearVelocity = vLinearVelocity;

	WakeUp();
}

void CRigidBody::SetAngularVelocity(const Vec3& vAngularVelocity)
{
	m_vAngularVelocity = vAngularVelocity;

	WakeUp();
}

CRigidBody* CRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRigidBody* pInstance = new CRigidBody(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRigidDynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRigidBody::Clone(CGameObject* pGameObject, void* pArg)
{
	CRigidBody* pInstance = new CRigidBody(*this);
	pInstance->m_pOwner = pGameObject;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CRigidDynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRigidBody::Free()
{
}