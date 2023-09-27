#pragma once

#include "Server_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CPipeLine;
END


BEGIN(Server)
class CPlayer_Server final : public CGameObject
{
public:
	typedef struct ModelDesc
	{
		wstring strFileName;
		_int	iObjectID;
	}MODELDESC;

public:
	enum STATE { STATE_IDLE, STATE_WALK, STATE_RUN, STATE_JUMP, STATE_END };
	enum PARTTYPE { PART_WEAPON, PART_END };

private:
	CPlayer_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Server(const CPlayer_Server& rhs);
	virtual ~CPlayer_Server() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	CShader* Get_ShaderCom() { return m_pShaderCom; }
	CTransform* Get_TransformCom() { return m_pTransformCom; }
	CModel* Get_ModelCom() { return m_pModelCom; }
	
	Vec3	Get_TargetPos() { return m_vTargetPos; }
	void	Set_TargetPos(Vec3 vTargetPos) { m_vTargetPos = vTargetPos; }


protected:
	virtual HRESULT Ready_Components() override;

private: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;

private:
	vector<CGameObject*>				m_Parts;
	typedef vector<CGameObject*>		PARTS;

private:
	Vec3 m_vTargetPos;

private:
	HRESULT Ready_Sockets();
	HRESULT Ready_PlayerParts();
	HRESULT Update_Weapon();

public:
	static CPlayer_Server* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;


};

END

