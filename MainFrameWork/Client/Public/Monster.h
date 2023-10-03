#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "StateMachine.h"
#include <atomic>

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CPipeLine;
class CSphereCollider;
class CHierarchyNode;
class CCollider;
END


BEGIN(Client)
class CMonster : public CGameObject
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
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	CShader*				Get_ShaderCom() { return m_pShaderCom; }
	CTransform*				Get_TransformCom() { return m_pTransformCom; }
	CModel*					Get_ModelCom() { return m_pModelCom; }

	void					Set_TargetPos(Vec3 vTargetPos) { m_vTargetPos.store(vTargetPos); }
	Vec3					Get_TargetPos() { return m_vTargetPos.load(); }

	void					Set_TargetMatrix(Matrix matTargetWorld) { m_matTargetWorld.store(matTargetWorld); }
	Matrix					Get_TargetMatrix() { return m_matTargetWorld.load(); }


	void					Set_MoveSpeed(_float fSpeed) { m_fMoveSpeed = fSpeed; }
	_float					Get_MoveSpeed() { return m_fMoveSpeed; }
	void					Add_MoveSpeed(_float fSpeed, _float fMaxSpeed) { m_fMoveSpeed += fSpeed; m_fMoveSpeed = min(m_fMoveSpeed, fMaxSpeed); }

	_bool					Is_Control() { return m_bControl; }
public:
	void			Set_NoneControlState(const wstring& szName);
	void			Reserve_Animation(_uint iAnimIndex, _float fChangeTime, _uint iStartFrame, _uint iChangeFrame);

protected:
	virtual HRESULT Ready_Components();



protected:
	atomic<Vec3>					m_vTargetPos;
	atomic<Matrix>					m_matTargetWorld;

	_float							m_fMoveSpeed = 0.0f;


	unordered_map<wstring, _uint>	m_BoneIndex;
protected: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CStateMachine* m_pStateMachine = nullptr;

public:
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();


};

END

