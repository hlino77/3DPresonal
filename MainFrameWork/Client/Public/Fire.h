#pragma once
#include "Effect.h"
BEGIN(Engine)
class CVIBuffer_Point;
END


BEGIN(Client)

class CFire : public CEffect
{
	// CGameObject을(를) 통해 상속됨

private:
	/* 원형을 생성할 때 */
	CFire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CFire(const CFire& rhs); /* 복사 생성자. */
	virtual ~CFire() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Instance(ID3D11Buffer* pInstanceBuffer, _uint iSize);
	virtual void Add_InstanceData(vector<Vec4>& BufferData) override;

	void		Appear(Vec3 vPos);
	void		Appear(Vec3 vPos, _float fLength, _float fTime);
protected: 





protected:
	virtual HRESULT Ready_Components();


private:
	void	Update_UV(_float fTimeDelta);


private:	
	CVIBuffer_Point* m_pVIBufferCom = nullptr;
	CTexture* m_pFireMaskTexture = { nullptr };
	CTexture* m_pFireDisolveTexture = { nullptr };

	Vec2			m_vScale;
	Vec4			m_vColor;
	Vec4			m_vBlurColor;


	_bool m_bEnd = false;



	Vec2 m_vUVIndex;
	Vec2 m_vUVMaxCount;
	_float m_fUVTime;
	_float m_fCurrUVTime;


	_float m_fDiscardAlpha;


	_float m_fAlphaWeight;

	_float m_fStartTime;
	_bool m_bStart = false;
public:
	virtual void Free() override;
	virtual CGameObject* Clone(void* pArg) override;
	static CFire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

};

END