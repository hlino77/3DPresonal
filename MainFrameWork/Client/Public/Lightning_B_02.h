#pragma once
#include "Effect.h"
BEGIN(Engine)
END


BEGIN(Client)

class CLightning_B_02 : public CEffect
{
	// CGameObject을(를) 통해 상속됨

private:
	/* 원형을 생성할 때 */
	CLightning_B_02(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CLightning_B_02(const CLightning_B_02& rhs); /* 복사 생성자. */
	virtual ~CLightning_B_02() = default;

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
	static CLightning_B_02* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

};

END