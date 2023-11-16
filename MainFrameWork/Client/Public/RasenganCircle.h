#pragma once
#include "Effect.h"
BEGIN(Engine)

END


BEGIN(Client)

class CPlayer_Naruto;
class CRasengan_StartSmoke;
class CRasengan_RunSmoke;

class CRasenganCircle : public CEffect
{
	// CGameObject을(를) 통해 상속됨

private:
	/* 원형을 생성할 때 */
	CRasenganCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CRasenganCircle(const CRasenganCircle& rhs); /* 복사 생성자. */
	virtual ~CRasenganCircle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


	void		Set_Owner(CPlayer_Naruto* pPlayer);
	CPlayer_Naruto* Get_Owner() { return m_pPlayer; }


	void		Appear();
	void		DisAppear();


	void		RunSmoke();

	void		BillBoard();


protected: 





protected:
	virtual HRESULT Ready_Components();



private:	
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
	CTexture* m_pRingTexture = nullptr;
	CTexture* m_pBaseTexture = nullptr;


	Vec3			m_vScale;
	Vec4			m_vColor;
	Vec4			m_vBlurColor;


	_bool m_bEnd = false;


	_uint m_iBoneIndex = 0;


	CPlayer_Naruto* m_pPlayer = nullptr;


	Matrix m_matOrigin;


	CRasengan_StartSmoke* m_pStartSmoke = nullptr;
	CRasengan_RunSmoke* m_pRunSmoke = nullptr;
public:
	virtual void Free() override;
	virtual CGameObject* Clone(void* pArg) override;
	static CRasenganCircle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

};

END