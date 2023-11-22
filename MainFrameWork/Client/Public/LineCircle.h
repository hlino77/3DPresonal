#pragma once
#include "Effect.h"
BEGIN(Engine)
class CVIBuffer_Point;
END


BEGIN(Client)

class CLineCircle : public CEffect
{
	// CGameObject을(를) 통해 상속됨

private:
	/* 원형을 생성할 때 */
	CLineCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CLineCircle(const CLineCircle& rhs); /* 복사 생성자. */
	virtual ~CLineCircle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Instance(ID3D11Buffer* pInstanceBuffer, _uint iSize);
	virtual void Add_InstanceData(vector<Vec4>& BufferData) override;

	void		Appear(Vec3 vPos, Vec4 vColor, Vec4 vBlurColor,_float fForce);
	void		Appear_OnlyUp(Vec3 vPos, Vec4 vColor, Vec4 vBlurColor, _float fForce);

protected: 





protected:
	virtual HRESULT Ready_Components();



private:	
	CVIBuffer_Point* m_pVIBufferCom = nullptr;

	Vec2			m_vScale;
	Vec4			m_vColor;
	Vec4			m_vBlurColor;


	mt19937_64								m_RandomNumber;
	uniform_real_distribution<float>		m_RandomScaleX;
	uniform_real_distribution<float>		m_RandomScaleY;
	uniform_real_distribution<float>		m_RandomForce;
	uniform_real_distribution<float>		m_RandomAngle;
	uniform_real_distribution<float>		m_RandomDir;
	uniform_real_distribution<float>		m_RandomLifeTime;


	_float m_fLifeTime;



	_bool m_bEnd = false;


public:
	virtual void Free() override;
	virtual CGameObject* Clone(void* pArg) override;
	static CLineCircle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

};

END