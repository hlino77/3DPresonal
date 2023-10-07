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
	CTransform* Get_TransformCom() { return m_pTransformCom; }

protected:
	virtual HRESULT Ready_Components() override;

public:
	static CCamera_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, wstring strObjTag);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

	
private:
	_bool m_bMouseMove = true;
	Vec3 m_vOffset;


	_float m_fSesitive;
	_float m_fCameraSpeed;

	_float m_fSpeedX, m_fSpeedY, m_fMaxSpeedX, m_fMaxSpeedY;
	_float m_fCurrSpeedX, m_fCurrSpeedY;
	_float m_fBreak;

	_float m_fPrevY;

	_float m_fCameraLength;

	class CPlayer* m_pPlayer = nullptr;
};

END