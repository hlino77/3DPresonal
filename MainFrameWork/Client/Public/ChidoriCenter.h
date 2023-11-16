#pragma once
#include "Effect.h"
BEGIN(Engine)
class CVIBuffer_Point;
END


BEGIN(Client)

class CPlayer_Sasuke;

class CChidoriCenter : public CEffect
{
	// CGameObject을(를) 통해 상속됨

private:
	/* 원형을 생성할 때 */
	CChidoriCenter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CChidoriCenter(const CChidoriCenter& rhs); /* 복사 생성자. */
	virtual ~CChidoriCenter() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


	void		Set_Owner(CPlayer_Sasuke* pPlayer);

	void		Appear();
	void		DisAppear();


	void		BillBoard();


protected: 





protected:
	virtual HRESULT Ready_Components();


private:
	void	Update_UV(_float fTimeDelta);


private:	
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

	Vec3			m_vScale;
	Vec4			m_vColor;
	Vec4			m_vBlurColor;


	_bool m_bEnd = false;


	_uint m_iBoneIndex = 0;


	CPlayer_Sasuke* m_pPlayer = nullptr;


	Matrix m_matOrigin;


	Vec2 m_vUVIndex;
	Vec2 m_vUVMaxCount;
	_float m_fUVTime;
	_float m_fCurrUVTime;


public:
	virtual void Free() override;
	virtual CGameObject* Clone(void* pArg) override;
	static CChidoriCenter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

};

END