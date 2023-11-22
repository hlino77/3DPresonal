#pragma once
#include "Monster.h"

BEGIN(Client)

class CMonster_Susanoo_Madara : public CMonster
{
private:
	CMonster_Susanoo_Madara(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Susanoo_Madara(const CMonster_Susanoo_Madara& rhs);
	virtual ~CMonster_Susanoo_Madara() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_float fTimeDelta) override;
	virtual void		LateTick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;


	void				Set_Colliders();

	void				Appear(Vec3 vPos, Vec3 vLook);

protected:
	virtual HRESULT Ready_Components() override;


private:
	Vec4 m_vBlurColor;
	

	Vec3 m_vTargetPos;


	_bool m_bEnd = false;
	
	_float m_fAlpha = 0.0f;
public:
	static CMonster_Susanoo_Madara* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;


};

END