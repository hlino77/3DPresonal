#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CPipeLine;
END


BEGIN(Client)
class CStaticModel final : public CGameObject
{
public:
	typedef struct ModelDesc
	{
		wstring strFileName;
		_uint	iLayer;
	}MODELDESC;

public:
	enum STATE { STATE_IDLE, STATE_WALK, STATE_RUN, STATE_JUMP, STATE_END };
	enum PARTTYPE { PART_WEAPON, PART_END };

private:
	CStaticModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJ_TYPE eObjType);
	CStaticModel(const CStaticModel& rhs);
	virtual ~CStaticModel() = default;

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



	HRESULT Add_ModelComponent(const wstring& strComName);
protected:
	virtual HRESULT Ready_Components() override;

private: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;

private:



private:

public:
	static CStaticModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, OBJ_TYPE eObjType);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;


};

END

