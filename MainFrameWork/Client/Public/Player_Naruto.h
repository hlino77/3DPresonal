#pragma once
#include "Player.h"

BEGIN(Client)

class CPlayer_Naruto : public CPlayer
{
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



	virtual	void	OnCollisionEnter(const _uint iColLayer, class CCollider* pOther) override;
	virtual	void	OnCollisionStay(const _uint iColLayer, class CCollider* pOther) override;
	virtual	void	OnCollisionExit(const _uint iColLayer, class CCollider* pOther) override;
public:
	void				Send_PlayerInfo();

	void				Set_Colliders();
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