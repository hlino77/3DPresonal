#pragma once
#include "Effect.h"
BEGIN(Engine)
class CVIBuffer_Point;
END


BEGIN(Client)

class CSmoke_09 : public CEffect
{
	// CGameObject��(��) ���� ��ӵ�

private:
	/* ������ ������ �� */
	CSmoke_09(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CSmoke_09(const CSmoke_09& rhs); /* ���� ������. */
	virtual ~CSmoke_09() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Instance(ID3D11Buffer* pInstanceBuffer, _uint iSize);
	virtual void Add_InstanceData(vector<Vec4>& BufferData) override;

	void		Appear(Vec3 vPos);

protected: 





protected:
	virtual HRESULT Ready_Components();


private:
	void	Update_UV(_float fTimeDelta);


private:	
	CVIBuffer_Point* m_pVIBufferCom = nullptr;

	Vec2			m_vScale;
	Vec4			m_vColor;
	Vec4			m_vBlurColor;


	_bool m_bEnd = false;



	Vec2 m_vUVIndex;
	Vec2 m_vUVMaxCount;
	_float m_fUVTime;
	_float m_fCurrUVTime;
	_float m_fDiscardAlpha;


	_float m_fAlphaWeight = 0.0f;
public:
	virtual void Free() override;
	virtual CGameObject* Clone(void* pArg) override;
	static CSmoke_09* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

};

END