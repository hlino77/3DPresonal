#pragma once
#include "Effect.h"
BEGIN(Engine)

END


BEGIN(Client)

class CExplosionRing : public CEffect
{
	// CGameObject��(��) ���� ��ӵ�

private:
	/* ������ ������ �� */
	CExplosionRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CExplosionRing(const CExplosionRing& rhs); /* ���� ������. */
	virtual ~CExplosionRing() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();



	void		Appear(Vec3 vPos, Vec3 vScale);
	void		DisAppear();




	void		BillBoard();


protected: 





protected:
	virtual HRESULT Ready_Components();



private:	
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;


	Vec3			m_vScale;
	Vec4			m_vColor;
	Vec4			m_vBlurColor;


	_bool m_bEnd = false;


	_uint m_iBoneIndex = 0;

public:
	virtual void Free() override;
	virtual CGameObject* Clone(void* pArg) override;
	static CExplosionRing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

};

END