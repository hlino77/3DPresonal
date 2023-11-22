#pragma once
#include "Effect.h"
BEGIN(Engine)
END


BEGIN(Client)

class CSmokeRing_Syuriken : public CEffect
{
	// CGameObject��(��) ���� ��ӵ�

private:
	/* ������ ������ �� */
	CSmokeRing_Syuriken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CSmokeRing_Syuriken(const CSmokeRing_Syuriken& rhs); /* ���� ������. */
	virtual ~CSmokeRing_Syuriken() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Instance(ID3D11Buffer* pInstanceBuffer, _uint iSize) override;

	virtual void	Add_InstanceData(vector<Vec4>& BufferData);

	void		Appear(Vec3 vPos, _float fBaseScale, Vec4 vColor, Vec4 vBlurColor, _float fTime);
	void		DisAppear();



protected: 





protected:
	virtual HRESULT Ready_Components();



private:
	Vec4 m_vColor;
	Vec4 m_vBlur;


	Vec3 m_vMaxScale;

	_bool m_bEnd = false;

	_float m_fEffectTime;
	_float m_fCurrTime;
public:
	virtual void Free() override;
	virtual CGameObject* Clone(void* pArg) override;
	static CSmokeRing_Syuriken* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

};

END