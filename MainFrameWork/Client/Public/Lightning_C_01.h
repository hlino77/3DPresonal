#pragma once
#include "Effect.h"
BEGIN(Engine)
END


BEGIN(Client)

class CLightning_C_01 : public CEffect
{
	// CGameObject��(��) ���� ��ӵ�

private:
	/* ������ ������ �� */
	CLightning_C_01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CLightning_C_01(const CLightning_C_01& rhs); /* ���� ������. */
	virtual ~CLightning_C_01() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Instance(ID3D11Buffer* pInstanceBuffer, _uint iSize) override;

	virtual void	Add_InstanceData(vector<Vec4>& BufferData);

	void		Appear(Vec3 vPos);
	void		DisAppear();


protected:
	virtual HRESULT Ready_Components();



private:
	Vec4 m_vColor;
	Vec4 m_vBlur;


	_bool m_bEnd = false;

	_float m_fUV = 0.0f;
	_float m_fUVSpeed = 0.0f;

	mt19937_64								m_RandomNumber;
	uniform_real_distribution<float>		m_RandomScale;
public:
	virtual void Free() override;
	virtual CGameObject* Clone(void* pArg) override;
	static CLightning_C_01* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

};

END