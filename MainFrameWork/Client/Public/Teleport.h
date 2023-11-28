#pragma once
#include "Effect.h"
BEGIN(Engine)
class CVIBuffer_Rect;
END


BEGIN(Client)

class CTeleport : public CEffect
{
	// CGameObject을(를) 통해 상속됨

private:
	/* 원형을 생성할 때 */
	CTeleport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CTeleport(const CTeleport& rhs); /* 복사 생성자. */
	virtual ~CTeleport() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void		Appear(Vec3 vPos, Vec3 vUp);

protected: 





protected:
	virtual HRESULT Ready_Components();


private:
	void		BillBoard();

private:	
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CTexture* m_pLineTexture = nullptr;



	Vec3			m_vTargetScale;
	Vec4			m_vColor;

	_bool m_bEnd = false;


	_float m_fAlphaWeight = 0.0f;


	_float m_fEffectTime = 0.0f;
	_float m_fCurrTime = 0.0f;
public:
	virtual void Free() override;
	virtual CGameObject* Clone(void* pArg) override;
	static CTeleport* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

};

END