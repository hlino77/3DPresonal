#pragma once

#include "Base.h"
#include "AsTypes.h"

/* Ŭ���̾���Ʈ���� ������ �پ��� ���ӿ�����Ʈ���� �θ𰡵ȴ�. */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	/* ������ ������ �� */
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, wstring strObjectTag, _int iObjType);

	/* �纻�� ������ �� */
	CGameObject(const CGameObject& rhs); /* ���� ������. */
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	class CComponent* Get_Component(const wstring & strComponentTag);

public:
	virtual HRESULT SetUp_State(Matrix StateMatrix) { return S_OK; }


public:
	void			Set_ObjectTag(const wstring& strName) { m_strObjectTag = strName; }
	const wstring&	Get_ObjectTag() { return m_strObjectTag; }

	_uint			Get_ObjectID() { return m_iObjectID; }

protected:
	virtual HRESULT Ready_Components() PURE;
	HRESULT Add_Component(_uint iLevelIndex, const wstring& pPrototypeTag, const wstring& pComponentTag, CComponent** ppOut, void* pArg = nullptr);
	HRESULT Compute_CamZ(Vec4 vWorldPos);


protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

protected:
	map<wstring, class CComponent*> m_Components;

protected:
	_float				m_fCamDistance = 0.f;
	wstring				m_strObjectTag = L"";


	_int				m_iObjType = -1;
	_int				m_iObjectID = -1;
	_bool				m_bControl = false;
private:
	CComponent* Find_Component(const wstring & strComponentTag);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END