#pragma once

#include "Base.h"

BEGIN(Engine)

/* 빛들을 보관한다. */

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
public:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHTDESC* Get_LightDesc(_uint iIndex);

public:
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
public:
	HRESULT Reset_Lights();
	void Ready_LightMatrix(Matrix matWorld) { m_DirectionLightMatrix = matWorld; }
	Matrix Get_DirectionLightMatrix() { return m_DirectionLightMatrix; }

private:
	

private:
	list<class CLight*>			m_Lights;
	typedef list<class CLight*>	LIGHTS;

	Matrix m_DirectionLightMatrix;
public:
	virtual void Free() override;
};

END