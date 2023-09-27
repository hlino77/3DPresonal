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

class CHierarchyNode;
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

	void											Set_TargetPos(Vec3 vTargetPos) { m_vTargetPos = vTargetPos; }
	DirectX::SimpleMath::Vector3					Get_TargetPos() { return m_vTargetPos; }

	void											Set_MoveSpeed(_float fSpeed) { m_fMoveSpeed = fSpeed; }
	_float											Get_MoveSpeed() { return m_fMoveSpeed; }
	void											Add_MoveSpeed(_float fSpeed, _float fMaxSpeed) { m_fMoveSpeed += fSpeed; m_fMoveSpeed = min(m_fMoveSpeed, fMaxSpeed); }

public:


	//Send Packet
	void			Send_Animation(_uint iAnimIndex, _float fChangeTime, _uint iStartFrame, _uint iChangeFrame);
	void			Send_WorldMatrix();


	void			Set_State(const wstring& szName);
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
	class CCamera_Player*	m_pCamera = nullptr;

	DirectX::SimpleMath::Vector3					m_vTargetPos;
	DirectX::SimpleMath::Matrix						m_matTargetWorld;

	_float					m_fMoveSpeed = 0.0f;
private:
	void			Make_WorldMatrix_Packet(Protocol::S_MATRIX& pkt);

protected: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CStateMachine* m_pStateMachine = nullptr;
private:
	vector<CGameObject*>				m_Parts;
	typedef vector<CGameObject*>		PARTS;

private:
	
	
private:
	HRESULT Ready_Sockets();
	HRESULT Ready_PlayerParts();
	HRESULT Update_Weapon();

public:
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();


};

END

