#pragma once
#include "Effect.h"

BEGIN(Engine)
class CVIBuffer_Point;
END


BEGIN(Client)

class CFootTrail : public CEffect
{
	// CGameObject을(를) 통해 상속됨
public:
	typedef struct TrailPointDesc
	{
		Matrix matPointData;
		Vec3 vPoint;
	}TRAILPOINT;

private:
	/* 원형을 생성할 때 */
	CFootTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CFootTrail(const CFootTrail& rhs); /* 복사 생성자. */
	virtual ~CFootTrail() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	void		Appear(CGameObject* pOwner, _uint iBoneIndex, Vec4 vColor, Vec4 vBlurColor);
	void		Disappear();
	void		Add_Point();
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
	_uint			m_iMaxSize;


	_bool m_bEnd = false;
	_bool m_bDisappear = false;
	



	list<TrailPointDesc> m_PointList;
	ID3D11Buffer* m_pInstanceBuffer = nullptr;
public:
	virtual void Free() override;
	virtual CGameObject* Clone(void* pArg) override;
	static CFootTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

};

END