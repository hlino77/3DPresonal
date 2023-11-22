#pragma once
#include "Effect.h"
BEGIN(Engine)

END


BEGIN(Client)

class CPlayer_Naruto;

class CRasenSyurikenCenter : public CEffect
{
	// CGameObject을(를) 통해 상속됨

private:
	/* 원형을 생성할 때 */
	CRasenSyurikenCenter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CRasenSyurikenCenter(const CRasenSyurikenCenter& rhs); /* 복사 생성자. */
	virtual ~CRasenSyurikenCenter() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


	void		Set_Owner(CGameObject* pOwner) { m_pOwner = pOwner; }


	void		Appear();
	void		DisAppear();


	void		BillBoard();


protected: 





protected:
	virtual HRESULT Ready_Components();

private:
	void		Update_Transform(_float fTimeDelta);

private:	
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CTexture* m_pRingTexture = nullptr;
	CTexture* m_pBaseTexture = nullptr;

	Vec4			m_vColor;
	Vec4			m_vBlurColor;


	Vec3 m_vCurrScale;
	Vec3 m_vTargetScale;


	_bool m_bEnd = false;


	_uint m_iBoneIndex = 0;

	CGameObject* m_pOwner = nullptr;

public:
	virtual void Free() override;
	virtual CGameObject* Clone(void* pArg) override;
	static CRasenSyurikenCenter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

};

END