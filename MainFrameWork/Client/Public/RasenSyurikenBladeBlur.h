#pragma once
#include "Effect.h"
BEGIN(Engine)
END


BEGIN(Client)

class CRasenSyurikenBladeBlur : public CEffect
{
	// CGameObject��(��) ���� ��ӵ�

private:
	/* ������ ������ �� */
	CRasenSyurikenBladeBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CRasenSyurikenBladeBlur(const CRasenSyurikenBladeBlur& rhs); /* ���� ������. */
	virtual ~CRasenSyurikenBladeBlur() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void		Appear();
	void		DisAppear();

	void		Set_Owner(CGameObject* pOwner) { m_pOwner = pOwner; }


protected:
	virtual HRESULT Ready_Components();


private:
	void		Update_Transform(_float fTimeDelta);


private:
	Vec4 m_vColor;
	Vec4 m_vBlur;

	_float m_fAlphaWeight = 0.0f;
	_float m_fDiscardAlpha = 0.0f;

	_bool m_bEnd = false;



	CGameObject* m_pOwner = nullptr;


	_float m_fTurnSpeed = 0.0f;
	_float m_fScaleSpeed = 0.0f;
	_float m_fCurrRadian = 0.0f;

	Vec3 m_vTargetScale;
	Vec3 m_vCurrScale;

public:
	virtual void Free() override;
	virtual CGameObject* Clone(void* pArg) override;
	static CRasenSyurikenBladeBlur* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

};

END