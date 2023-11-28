#pragma once
#include "Effect.h"
BEGIN(Engine)
END


BEGIN(Client)

class CSkyBoxNight : public CEffect
{
	// CGameObject��(��) ���� ��ӵ�

private:
	/* ������ ������ �� */
	CSkyBoxNight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CSkyBoxNight(const CSkyBoxNight& rhs); /* ���� ������. */
	virtual ~CSkyBoxNight() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	virtual HRESULT Ready_Components();


private:



private:


	_float m_fUVX;

public:
	virtual void Free() override;
	virtual CGameObject* Clone(void* pArg) override;
	static CSkyBoxNight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

};

END