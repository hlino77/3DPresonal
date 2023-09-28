#pragma once
#include "Player.h"

BEGIN(Client)

class CPlayer_Naruto : public CPlayer
{
public:
	enum STATE { STATE_IDLE, STATE_WALK, STATE_RUN, STATE_JUMP, STATE_END };
	enum PARTTYPE { PART_WEAPON, PART_END };

private:
	CPlayer_Naruto(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Naruto(const CPlayer_Naruto& rhs);
	virtual ~CPlayer_Naruto() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void	Send_PlayerInfo();

protected:
	virtual HRESULT Ready_Components() override;
	HRESULT Ready_State();

private:
	_float	m_fSendInfoTime = 0.0f;

public:
	static CPlayer_Naruto* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;


};

END