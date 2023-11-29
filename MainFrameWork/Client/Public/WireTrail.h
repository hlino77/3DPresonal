#pragma once
#include "Effect.h"

BEGIN(Engine)
class CVIBuffer_Point;
END


BEGIN(Client)

class CWireTrail : public CEffect
{
public:
	// CGameObject을(를) 통해 상속됨
	typedef struct WireTrailDesc
	{
		CGameObject* pOwner;
		_uint iBoneIndex;

	}WIRETRAIL;



private:
	/* 원형을 생성할 때 */
	CWireTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CWireTrail(const CWireTrail& rhs); /* 복사 생성자. */
	virtual ~CWireTrail() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void		Appear();
	void		Disappear();
	void		Update_Point();


	_bool		Is_Disappear() { return m_bDisappear; }
protected:
	virtual HRESULT Ready_Components();



private:
	HRESULT Ready_InstanceBuffer();


private:	
	CVIBuffer_Point* m_pVIBufferCom = nullptr;

	Vec4			m_vColor;
	Vec4			m_vBlurColor;
	_uint			m_iBoneIndex;
	CGameObject*	m_pOwner = nullptr;



	_bool m_bEnd = false;
	_bool m_bDisappear = false;
	
	ID3D11Buffer* m_pInstanceBuffer = nullptr;
	Matrix matTrailData;

public:
	virtual void Free() override;
	virtual CGameObject* Clone(void* pArg) override;
	static CWireTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

};

END