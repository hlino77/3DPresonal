#pragma once
#include "Effect.h"
BEGIN(Engine)
END


BEGIN(Client)

class CSkyBoxDay : public CEffect
{
	// CGameObject을(를) 통해 상속됨

private:
	/* 원형을 생성할 때 */
	CSkyBoxDay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CSkyBoxDay(const CSkyBoxDay& rhs); /* 복사 생성자. */
	virtual ~CSkyBoxDay() = default;

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
	static CSkyBoxDay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

};

END