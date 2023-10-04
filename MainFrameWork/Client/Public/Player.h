#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "StateMachine.h"

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
class CPlayer : public CGameObject
{
public:
	typedef struct ModelDesc
	{
		wstring strFileName;
		_bool	bControl;
		_int	iObjectID;
		_uint	iLayer;
	}MODELDESC;

public:


protected:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

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

	void					Set_Camera(class CCamera_Player* pCamera) { m_pCamera = pCamera; }
	class CCamera_Player*	Get_Camera() { return m_pCamera; }

	void					Set_TargetPos(Vec3 vTargetPos) { m_vTargetPos.store(vTargetPos); }
	Vec3					Get_TargetPos() { return m_vTargetPos.load(); }

	void					Set_TargetMatrix(Matrix matTargetWorld) { m_matTargetWorld.store(matTargetWorld); }
	Matrix					Get_TargetMatrix() { return m_matTargetWorld.load(); }


	void					Set_MoveSpeed(_float fSpeed) { m_fMoveSpeed = fSpeed; }
	_float					Get_MoveSpeed() { return m_fMoveSpeed; }
	void					Add_MoveSpeed(_float fSpeed, _float fMaxSpeed) { m_fMoveSpeed += fSpeed; m_fMoveSpeed = min(m_fMoveSpeed, fMaxSpeed); }

	_bool					Is_Control() { return m_bControl; }
public:


	//Send Packet
	void			Send_Animation(_uint iAnimIndex, _float fChangeTime, _uint iStartFrame, _uint iChangeFrame);
	void			Send_State(const wstring& szName);
	void			Send_ColliderState(const _uint& iLayer);

	void			Set_State(const wstring& szName);
	void			Set_NoneControlState(const wstring& szName);
	void			Reserve_Animation(_uint iAnimIndex, _float fChangeTime, _uint iStartFrame, _uint iChangeFrame);




	Vec3			Make_StraightDir();
	Vec3			Make_RightDir();
	Vec3			Make_BackDir();
	Vec3			Make_LeftDir();
	void			Go_Straight(_float fSpeed, _float fTimeDelta);
	void			Move_Dir(Vec3 vDir, _float fSpeed, _float fTimeDelta);
protected:
	virtual HRESULT Ready_Components();



protected:
	class CCamera_Player*			m_pCamera = nullptr;

	atomic<Vec3>					m_vTargetPos;
	atomic<Matrix>					m_matTargetWorld;

	_float							m_fMoveSpeed = 0.0f;


	unordered_map<wstring, _uint>	m_BoneIndex;
protected: /* �ش� ��ü�� ����ؾ��� ������Ʈ���� �����ϳ�. */
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

