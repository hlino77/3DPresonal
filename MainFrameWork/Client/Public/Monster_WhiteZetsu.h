#pragma once
#include "Monster.h"

BEGIN(Client)

class CMonster_WhiteZetsu : public CMonster
{
private:
	CMonster_WhiteZetsu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_WhiteZetsu(const CMonster_WhiteZetsu& rhs);
	virtual ~CMonster_WhiteZetsu() = default;

public:
	virtual HRESULT		Initialize_Prototype();
	virtual HRESULT		Initialize(void* pArg);
	virtual void		Tick(_float fTimeDelta);
	virtual void		LateTick(_float fTimeDelta);
	virtual HRESULT		Render();

	void				Set_Colliders();

protected:
	virtual HRESULT Ready_Components() override;
	HRESULT Ready_State();

private:
	_float	m_fSendInfoTime = 0.0f;

public:
	static CMonster_WhiteZetsu* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;


};

END