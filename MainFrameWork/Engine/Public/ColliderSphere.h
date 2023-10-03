#pragma once
#include "ColliderBase.h"

BEGIN(Engine)

class ENGINE_DLL CSphereCollider : public CCollider
{
	using Super = CCollider;
private:
	CSphereCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSphereCollider(const CSphereCollider& rhs);
	virtual ~CSphereCollider() = default;

public:
	virtual HRESULT Initialize_Prototype()					override;
	virtual HRESULT Initialize(void* pArg)					override;
	virtual void	Tick(const _float& fTimeDelta)			override;
	virtual void	LateTick(const _float& fTimeDelta)		override;
	virtual void	DebugRender()							override;

	virtual _bool	Intersects(SimpleMath::Ray& ray, OUT _float& distance)	override;
	virtual _bool	Intersects(Super* other)					override;

	
	void			Set_Center(Vec3 vPos) { m_tBoundingSphere.Center = vPos; }
	void			Set_Center_ToBone(Matrix matBone);
	Vec3			Get_Center() { return m_tBoundingSphere.Center; }
	void			SetRadius(_float radius) { m_fRadius = radius; m_tBoundingSphere.Radius - m_fRadius; }
	_float			Get_Radius() { return m_tBoundingSphere.Radius; }

	BoundingSphere&	GetBoundingSphere()		{ return m_tBoundingSphere; }

private:
	_float			m_fRadius = 1.f;
	BoundingSphere	m_tBoundingSphere;

private:
	CTransform*		m_pOwnerTransform = nullptr;


public:
	static	CSphereCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pGameObject, void* pArg)	override;
	virtual void Free()						override;
};

END