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
class CPlayer_Lobby : public CGameObject
{
protected:
	CPlayer_Lobby(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Lobby(const CPlayer_Lobby& rhs);
	virtual ~CPlayer_Lobby() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();



	void			Set_CharacterName(const wstring& szCharacterName);
public:
	CShader*				Get_ShaderCom() { return m_pShaderCom; }

protected:
	virtual HRESULT			Ready_Components();


protected: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	

	unordered_map<wstring, class CModel*> m_Models;

	wstring m_szCharacterName;

	Matrix m_matView, m_matProj;
public:
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();

	static CPlayer_Lobby* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

};

END

