#pragma once
#include "Component.h"
#include <atomic>

BEGIN(Engine)

enum class ColliderType
{
	Sphere,
	AABB,
	OBB,
	Cylinder
};

class CRigidBody;

class ENGINE_DLL CCollider : public CComponent
{
	using Super = CComponent;
public:
	struct ColliderInfo
	{
		CGameObject* pOwner = nullptr;
		_uint m_iLayer;
		_bool m_bActive = false;
	};

protected:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ColliderType eColliderType);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype()						override;
	virtual HRESULT Initialize(void* pArg)						override;
	virtual void	DebugRender()								override;


	virtual _bool	Intersects(DirectX::SimpleMath::Ray& ray, OUT float& distance)	PURE;
	virtual _bool	Intersects(CCollider* other)				PURE;

	ColliderType	GetColliderType()			{ return m_eColliderType; }
	_uint			GetID()						{ return m_iID; }
	CRigidBody*		GetRigidBody()				{ return m_pRigidBody; }
	_uint			Get_ColLayer()				{ return m_iColLayer; }
	CGameObject*	Get_Owner()					{ return m_pOwner; }

	_bool			IsTrigger()					{ return m_IsTrigger; }
	void			SetTrigger(_bool IsTrigger) { m_IsTrigger = IsTrigger; }

	_bool			IsActive()					{ return m_bActive; }
	void			SetActive(_bool bActive)	{ m_bActive = bActive; }

	void			Set_BoneIndex(_uint iBoneIndex) { m_iBoneIndex = iBoneIndex; }
	_uint			Get_BoneIndex() { return m_iBoneIndex; }

	void			OnCollisionEnter(CCollider* pOther);
	void			OnCollisionExit(CCollider* pOther);
protected:
	CRigidBody*				m_pRigidBody;
	CGameObject*			m_pOwner = nullptr;

	_uint					m_iColLayer;
	_bool					m_bActive = false;

	_uint					m_iBoneIndex = 0;
private:
	ColliderType			m_eColliderType;
	_bool					m_IsTrigger;

	_uint					m_iID;
	static atomic<_uint>	g_iNextID;
	
	//_int			m_iCol;


	
#ifdef _DEBUG
protected:
	PrimitiveBatch<VertexPositionColor>*	m_pBatch = nullptr;
	BasicEffect*							m_pEffect = nullptr;
	ID3D11InputLayout*						m_pInputLayout = nullptr;
#endif

public:
	virtual void Free()						override;
};

END