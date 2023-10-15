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

	void					Set_Camera(class CCamera_Player* pCamera) { m_pCamera = pCamera; }
	class CCamera_Player*	Get_Camera() { return m_pCamera; }

	


	void					Set_MoveSpeed(_float fSpeed) { m_fMoveSpeed = fSpeed; }
	_float					Get_MoveSpeed() { return m_fMoveSpeed; }
	void					Add_MoveSpeed(_float fSpeed, _float fMaxSpeed) { m_fMoveSpeed += fSpeed; m_fMoveSpeed = min(m_fMoveSpeed, fMaxSpeed); }


	void					Set_DoubleJump(_bool bJump) { m_bDoubleJump = bJump; }
	_bool					Is_DoubleJump() { return m_bDoubleJump; }
public:


	//Send Packet
	void			Send_Animation(_uint iAnimIndex, _float fChangeTime, _uint iStartFrame, _uint iChangeFrame);
	void			Send_State(const wstring& szName);
	void			Send_ColliderState(const _uint& iLayer);

	void			Set_State(const wstring& szName);
	void			Reserve_Animation(_uint iAnimIndex, _float fChangeTime, _uint iStartFrame, _uint iChangeFrame);


	void					Find_NearTarget();

	Vec3			Make_StraightDir();
	Vec3			Make_RightDir();
	Vec3			Make_BackDir();
	Vec3			Make_LeftDir();

	Vec3			Make_Straight_JumpDir();
	Vec3			Make_Right_JumpDir();
	Vec3			Make_Back_JumpDir();
	Vec3			Make_Left_JumpDir();


	void			Go_Straight(_float fSpeed, _float fTimeDelta);
	void			Move_Dir(Vec3 vDir, _float fSpeed, _float fTimeDelta);


	void			Follow_ServerPos(_float fDistance, _float fLerpSpeed);
protected:
	virtual HRESULT Ready_Components();



protected:
	class CCamera_Player*			m_pCamera = nullptr;

	

	_float							m_fMoveSpeed = 0.0f;


	unordered_map<wstring, _uint>	m_BoneIndex;
	_bool							m_bDoubleJump = true;

	
protected: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	

public:
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();


};

END

