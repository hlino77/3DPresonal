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


	virtual	void	OnCollisionEnter(const _uint iColLayer, class CCollider* pOther) {};
	virtual	void	OnCollisionStay(const _uint iColLayer, class CCollider * pOther) {};
	virtual	void	OnCollisionExit(const _uint iColLayer, class CCollider * pOther) {};
public:
	class CComponent*				Get_Component(const wstring & strComponentTag);

	class CTransform*	Get_TransformCom() { return m_pTransformCom; }
	class CModel*		Get_ModelCom() { return m_pModelCom; }
	class CRigidBody*	Get_RigidBody() { return m_pRigidBody; }
public:
	virtual HRESULT			SetUp_State(Matrix StateMatrix) { return S_OK; }
	void					Set_NoneControlState(const wstring& szName);

public:
	void					Set_TargetPos(Vec3 vTargetPos) { m_vTargetPos.store(vTargetPos); }
	Vec3					Get_TargetPos() { return m_vTargetPos.load(); }

	void					Set_TargetMatrix(Matrix matTargetWorld) { m_matTargetWorld.store(matTargetWorld); }
	Matrix					Get_TargetMatrix() { return m_matTargetWorld.load(); }

	void					Set_ObjectTag(const wstring& strName) { m_strObjectTag = strName; }
	const wstring&			Get_ObjectTag() { return m_strObjectTag; }

	_uint					Get_ObjectID() { return m_iObjectID; }
	_uint					Get_ObjectLayer() { return m_iLayer; }

	class CSphereCollider*	Get_Colider(const _uint& iLayer) { return m_Coliders[iLayer]; }

	_bool					Is_Control() { return m_bControl; }


	CGameObject*			Get_NearTarget() { return m_pNearTarget; }
	void					Reset_NearTarget() { m_pNearTarget = nullptr; }
	void					Set_NearTarget(CGameObject* pObject) { m_pNearTarget = pObject; }

	CGameObject*			Get_HitObject() { return m_pHitObject; }
	void					Reset_HitObject() { m_pHitObject = nullptr; }
	void					Set_HitObject(CGameObject* pObject) { m_pHitObject = pObject; }
protected:
	virtual HRESULT Ready_Components() PURE;
	HRESULT Add_Component(_uint iLevelIndex, const wstring& pPrototypeTag, const wstring& pComponentTag, CComponent** ppOut, void* pArg = nullptr);
	HRESULT Compute_CamZ(Vec4 vWorldPos);


protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

protected:
	map<wstring, class CComponent*> m_Components;

	class CTransform* m_pTransformCom = nullptr;
	class CModel* m_pModelCom = nullptr;
	class CStateMachine* m_pStateMachine = nullptr;
	class CRigidBody* m_pRigidBody = nullptr;


	unordered_map<_uint, class CSphereCollider*> m_Coliders;
protected:
	_float				m_fCamDistance = 0.f;
	wstring				m_strObjectTag = L"";


	_int				m_iObjType = -1;
	_int				m_iObjectID = -1;
	_bool				m_bControl = false;
	_uint				m_iLayer = 0;


	atomic<Vec3>					m_vTargetPos;
	atomic<Matrix>					m_matTargetWorld;

	CGameObject* m_pNearTarget = nullptr;
	CGameObject* m_pHitObject = nullptr;
private:
	CComponent* Find_Component(const wstring & strComponentTag);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END