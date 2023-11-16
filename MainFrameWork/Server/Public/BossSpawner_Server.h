#pragma once

#include "Server_Defines.h"
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


BEGIN(Server)
class CBossSpawner_Server : public CGameObject
{
private:
	CBossSpawner_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossSpawner_Server(const CBossSpawner_Server& rhs);
	virtual ~CBossSpawner_Server() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


	virtual	void	OnCollisionEnter(const _uint iColLayer, class CCollider* pOther) override;
	virtual	void	OnCollisionStay(const _uint iColLayer, class CCollider* pOther) override;
	virtual	void	OnCollisionExit(const _uint iColLayer, class CCollider* pOther) override;
	void			Send_Collision(_uint iColLayer, CCollider* pOther, _bool bEnter);



	void					Set_Die();



	


	void					Set_Spawner(Vec3 vPos, _float fRadius);
private:
	void					Spawn_Boss();

private:



	virtual HRESULT			Ready_Components() override;



	_float					m_fSpawnDelay = 0.0f;
	_bool					m_bSpawn = false;





public:
	virtual void Free();
	static	CBossSpawner_Server* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
};

END

