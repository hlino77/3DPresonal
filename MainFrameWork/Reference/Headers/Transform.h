#pragma once

#include "Component.h"
#include "AsTypes.h"
#include "CoreMacro.h"
#include "Lock.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

	typedef struct tagTransformDesc
	{
		_float			fSpeedPerSec;
		_float			fRotationPerSec;
	}TRANSFORMDESC;


private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

	USE_LOCK
public:
	Vec3 Get_State(STATE eState) {
		READ_LOCK
		return Vec3(m_WorldMatrix.m[eState][0], m_WorldMatrix.m[eState][1], m_WorldMatrix.m[eState][2]);
	}

	Matrix Get_WorldMatrix() {
		READ_LOCK
		return m_WorldMatrix;
	}


	Matrix Get_WorldMatrix_TP() {
		READ_LOCK
		return m_WorldMatrix.Transpose();
	}

	Matrix Get_WorldMatrixInverse() {
		READ_LOCK
		return m_WorldMatrix.Invert();
	}

	void Set_State(STATE eState, Vec3 vState);
	void Set_WorldMatrix(Matrix matWorld) {
		WRITE_LOCK
		m_WorldMatrix = matWorld; 
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void Go_Straight(_float fSpeed, _float fTimeDelta);
	void Go_Backward(_float fSpeed, _float fTimeDelta);
	void Go_Left(_float fSpeed, _float fTimeDelta);
	void Go_Right(_float fSpeed, _float fTimeDelta);
	void Go_Up(_float fTimeDelta);
	void Go_Down(_float fTimeDelta);

	void Set_Scale(Vec3 vScaleInfo);
	Vec3 Get_Scale();

	void Turn(Vec3 vAxis, _float fTimeDelta);
	void Turn_Speed(Vec3 vAxis, _float fSpeed, _float fTimeDelta);
	void Rotation(Vec3 vAxis, _float fRadian);

	void LookAt(Vec3 vAt);
	void LookAt_ForLandObject(Vec3 vAt);
	void Move(Vec3 vTargetPos, _float fTimeDelta, _float fLimitDistance = 0.1f);
	void Move_Dir(Vec3 vDir, _float fTimeDelta);
	void LookAt_Lerp(Vec3 vAt, _float fSpeed, _float fTimeDelta);

private:
	Matrix					m_WorldMatrix;
	TRANSFORMDESC			m_TransformDesc;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END