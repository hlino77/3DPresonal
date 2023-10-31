#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)


class CCamera_Player final : public CCamera
{
public:
	typedef struct PlayerCameraDesc
	{
		class CPlayer* pPlayer = nullptr;
		_float fSensitive;
		_float fSpeed;
		CCamera::CAMERADESC tCameraDesc;
		Vec3 vInitPos;
	};

private:
	CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, wstring strObjTag);
	CCamera_Player(const CCamera_Player& rhs, CTransform::TRANSFORMDESC* pArg);
	virtual ~CCamera_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	void		Cam_Shake(_float fForce, _float fTime);



protected:
	virtual HRESULT Ready_Components() override;


	
private:
	_bool m_bMouseMove = true;
	Vec3 m_vTargetOffset;
	_float m_fCameraAngle;


	_float m_fSesitive;
	_float m_fCameraSpeed;

	_float m_fSpeedX, m_fSpeedY, m_fMaxSpeedX, m_fMaxSpeedY;
	_float m_fCurrSpeedX, m_fCurrSpeedY;
	_float m_fBreak;

	_float m_fPrevY;

	_float m_fCameraLength;


	//CamShake
	_float	m_fShakeTime = 0.0f;
	_float	m_fCurrShakeTime = 0.0f;
	_float	m_fShakeForce = 0.0f;
	_bool	m_bShake = false;
	Vec3	m_vOriginLook;




	class CPlayer* m_pPlayer = nullptr;


public:
	static CCamera_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, wstring strObjTag);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END