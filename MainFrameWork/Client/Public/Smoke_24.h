#pragma once
#include "Effect.h"
BEGIN(Engine)
class CVIBuffer_Point;
END


BEGIN(Client)

class CSmoke_24 : public CEffect
{
	// CGameObject을(를) 통해 상속됨

private:
	/* 원형을 생성할 때 */
	CSmoke_24(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CSmoke_24(const CSmoke_24& rhs); /* 복사 생성자. */
	virtual ~CSmoke_24() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Instance(ID3D11Buffer* pInstanceBuffer, _uint iSize);
	virtual void Add_InstanceData(vector<Vec4>& BufferData) override;

	void		Appear(Vec3 vPos, Vec4 vColor, Vec2 vScale, _float fTime, _float fDirForce, _float fStartTime);
	void		Appear_Up(Vec3 vPos, Vec4 vColor, Vec2 vScale, _float fTime, _float fDirForce, _float fStartTime);
protected: 





protected:
	virtual HRESULT Ready_Components();


private:


private:	
	CVIBuffer_Point* m_pVIBufferCom = nullptr;
	CTexture* m_pDisolveTexture = { nullptr };

	Vec2			m_vScale;
	Vec4			m_vColor;
	Vec4			m_vBlurColor;


	_bool m_bEnd = false;



	Vec2 m_vUVIndex;
	Vec2 m_vUVMaxCount;


	_float m_fDiscardAlpha;
	_float m_fAlphaWeight;


	_float m_fEffectTime;

	_float m_fSpeed;
	Vec3 m_vDir;

	_bool m_bStart = false;
	_float m_fStartTime = 0.0f;
public:
	virtual void Free() override;
	virtual CGameObject* Clone(void* pArg) override;
	static CSmoke_24* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

};

END