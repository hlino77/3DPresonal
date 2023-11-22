#pragma once
#include "Effect.h"
BEGIN(Engine)
END


BEGIN(Client)

class CRasenSyurikenSphere : public CEffect
{
	// CGameObject��(��) ���� ��ӵ�

private:
	/* ������ ������ �� */
	CRasenSyurikenSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CRasenSyurikenSphere(const CRasenSyurikenSphere& rhs); /* ���� ������. */
	virtual ~CRasenSyurikenSphere() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void		Appear(Vec3 vPos);
	void		DisAppear();

	void		Set_Owner(CGameObject* pOwner) { m_pOwner = pOwner; }


protected:
	virtual HRESULT Ready_Components();


private:



private:
	CTexture* m_pDisolveTexture = { nullptr };

	Vec4 m_vColor;
	Vec4 m_vBlur;

	_bool m_bEnd = false;

	_float m_fAlphaWeight = 0.0f;

	CGameObject* m_pOwner = nullptr;

	CTexture* m_pOutTextureCom = { nullptr };

	_float m_fUVX;

	Vec3 m_vTargetScale;

	_float m_fEffectTime;


	_float m_fSmokeDelay = 0.0f;
	_float m_fCurrTime = 0.0f;

public:
	virtual void Free() override;
	virtual CGameObject* Clone(void* pArg) override;
	static CRasenSyurikenSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

};

END