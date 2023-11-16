#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "StateMachine.h"
#include <atomic>

BEGIN(Engine)
class CTransform;
class CSphereCollider;
class CCollider;
class CRenderer;
class CModel;
END


BEGIN(Client)
class CMonsterSpawner : public CGameObject
{
private:
	CMonsterSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterSpawner(const CMonsterSpawner& rhs);
	virtual ~CMonsterSpawner() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


	virtual	void	OnCollisionEnter(const _uint iColLayer, class CCollider* pOther) override;
	virtual	void	OnCollisionStay(const _uint iColLayer, class CCollider* pOther) override;
	virtual	void	OnCollisionExit(const _uint iColLayer, class CCollider* pOther) override;



	void					Set_Die();
private:
	virtual HRESULT			Ready_Components() override;



private:
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;


private:

	Vec4 m_vColor;
	Vec4 m_vBlurColor;

	_float m_fTurnSpeed;

	_bool m_bEnter = false;

	Vec3 m_vTargetScale;
public:
	virtual void Free();
	static	CMonsterSpawner* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
};

END

