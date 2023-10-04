#pragma once

#include "Server_Defines.h"
#include "GameObject.h"
#include "StateMachine.h"
#include <atomic>

BEGIN(Engine)
class CTransform;
class CSphereCollider;
class CCollider;
class CModel;
END


BEGIN(Server)
class CMonster_Server : public CGameObject
{
public:
	typedef struct ModelDesc
	{
		wstring strFileName;
		_int	iObjectID;
		_uint	iLayer;
	}MODELDESC;

public:


protected:
	CMonster_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Server(const CMonster_Server& rhs);
	virtual ~CMonster_Server() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


	virtual	void	OnCollisionEnter(const _uint iColLayer, class CCollider* pOther) override;
	virtual	void	OnCollisionStay(const _uint iColLayer, class CCollider* pOther) override;
	virtual	void	OnCollisionExit(const _uint iColLayer, class CCollider* pOther) override;
public:

	CModel*					Get_ModelCom() { return m_pModelCom; }
	CTransform*				Get_TransformCom() { return m_pTransformCom; }

	void					Set_TargetPos(Vec3 vTargetPos) { m_vTargetPos.store(vTargetPos); }
	Vec3					Get_TargetPos() { return m_vTargetPos.load(); }

	void					Set_TargetMatrix(Matrix matTargetWorld) { m_matTargetWorld.store(matTargetWorld); }
	Matrix					Get_TargetMatrix() { return m_matTargetWorld.load(); }


	void					Set_MoveSpeed(_float fSpeed) { m_fMoveSpeed = fSpeed; }
	_float					Get_MoveSpeed() { return m_fMoveSpeed; }
	void					Add_MoveSpeed(_float fSpeed, _float fMaxSpeed) { m_fMoveSpeed += fSpeed; m_fMoveSpeed = min(m_fMoveSpeed, fMaxSpeed); }



	
public:

	//Send Packet
	void				Send_State(const wstring& szName);
	void				Set_State(const wstring& szName);
	void				Set_NoneControlState(const wstring& szName);
	void				Reserve_Animation(_uint iAnimIndex, _float fChangeTime, _uint iStartFrame, _uint iChangeFrame);
	void				Send_ColliderState(const _uint& iLayer);

	void				Set_Colliders();
	HRESULT				Ready_Coliders();
protected:
	virtual HRESULT Ready_Components();



protected:
	atomic<Vec3>					m_vTargetPos;
	atomic<Matrix>					m_matTargetWorld;

	_float							m_fMoveSpeed = 0.0f;

	unordered_map<wstring, _uint>	m_BoneIndex;
protected: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	CModel* m_pModelCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CStateMachine* m_pStateMachine = nullptr;
	CSphereCollider* m_pBodySphere = nullptr;


public:
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();


};

END

