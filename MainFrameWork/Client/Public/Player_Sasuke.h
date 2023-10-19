#pragma once
#include "Player.h"

BEGIN(Client)

class CPlayer_Sasuke : public CPlayer
{
private:
	CPlayer_Sasuke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Sasuke(const CPlayer_Sasuke& rhs);
	virtual ~CPlayer_Sasuke() = default;

public:
	virtual HRESULT		Initialize_Prototype();
	virtual HRESULT		Initialize(void* pArg);
	virtual void		Tick(_float fTimeDelta);
	virtual void		LateTick(_float fTimeDelta);
	virtual HRESULT		Render();


	virtual	void	OnCollisionEnter(const _uint iColLayer, class CCollider* pOther) override;
	virtual	void	OnCollisionStay(const _uint iColLayer, class CCollider* pOther) override;
	virtual	void	OnCollisionExit(const _uint iColLayer, class CCollider* pOther) override;
public:
	void				Send_PlayerInfo();


	void				Set_Colliders(_float fTimeDelta);
protected:
	virtual HRESULT Ready_Components() override;
	HRESULT			Ready_State();
	HRESULT			Ready_Coliders();



private:
	_float	m_fSendInfoTime = 0.0f;

public:
	static CPlayer_Sasuke* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;


};

END