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
		_uint	iLayer;
		class CGameSession* pGameSession = nullptr;
	}MODELDESC;
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


	virtual	void	OnCollisionEnter(const _uint iColLayer, class CCollider* pOther) override;
	virtual	void	OnCollisionStay(const _uint iColLayer, class CCollider* pOther) override;
	virtual	void	OnCollisionExit(const _uint iColLayer, class CCollider* pOther) override;


	void Send_CamShake(_float fShakeForce, _float fShakeTime);
public:
	CShader* Get_ShaderCom() { return m_pShaderCom; }


public:
	void				Set_Colliders(_float fTimeDelta);


private:
	HRESULT				Ready_Animations();
	HRESULT				Ready_SasukeAnimation();
	HRESULT				Ready_NarutoAnimation();

protected:
	virtual HRESULT Ready_Components() override;

private: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	std::future<HRESULT>	m_PlayAnimation;

	class CGameSession* m_pGameSession = nullptr;
private:
	vector<CGameObject*>				m_Parts;
	typedef vector<CGameObject*>		PARTS;

public:
	static CPlayer_Server* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;


};

END

