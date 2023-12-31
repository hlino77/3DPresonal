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

class CUI_Hits;
class CFootTrail;
class CWireTrail;

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

	typedef struct SkillInfoTag
	{
		_float m_fCoolTime;
		_float m_fCurrCoolTime;
		_bool m_bReady;
	}SKILLINFO;




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
	virtual HRESULT Render_ShadowDepth();
	virtual void Set_SlowMotion(_bool bSlow) override;


	virtual	void	OnCollisionEnter_NoneControl(const _uint iColLayer, class CCollider* pOther) PURE;
	virtual	void	OnCollisionExit_NoneControl(const _uint iColLayer, class CCollider* pOther) PURE;
public:
	CShader*				Get_ShaderCom() { return m_pShaderCom; }

	void					Set_Camera(class CCamera_Player* pCamera) { m_pCamera = pCamera; }
	class CCamera_Player*	Get_Camera() { return m_pCamera; }

	void					Set_MoveSpeed(_float fSpeed) { m_fMoveSpeed = fSpeed; }
	_float					Get_MoveSpeed() { return m_fMoveSpeed; }
	void					Add_MoveSpeed(_float fSpeed, _float fMaxSpeed) { m_fMoveSpeed += fSpeed; m_fMoveSpeed = min(m_fMoveSpeed, fMaxSpeed); }

	void					Set_AttackMoveSpeed(_float fSpeed) { m_fAttackMoveSpeed = fSpeed; }
	_float					Get_AttackMoveSpeed() { return m_fAttackMoveSpeed; }

	void					Set_AnimationSpeed(_float fSpeed) { m_fAnimationSpeed = fSpeed; }
	_float					Get_AnimationSpeed() { return m_fAnimationSpeed; }

	void					Set_DoubleJump(_bool bJump) { m_bDoubleJump = bJump; }
	_bool					Is_DoubleJump() { return m_bDoubleJump; }
public:


	//Send Packet
	void			Send_Animation(_uint iAnimIndex, _float fChangeTime, _uint iStartFrame, _uint iChangeFrame);
	void			Send_State(const wstring& szName);
	void			Send_ColliderState(const _uint& iLayer);
	void			Send_SlowMotion(_bool bSlow);
	void			Send_MakeSkill(const wstring& szSkillName);
	void			Send_Hp();


	void			Set_State(const wstring& szName);
	void			Reserve_Animation(_uint iAnimIndex, _float fChangeTime, _uint iStartFrame, _uint iChangeFrame);


	void			Find_NearTarget();
	void			Send_NearTarget();

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

	void			Set_Picking(_bool bPicking) { m_bPicking = bPicking; }


	void			Set_Wall(_bool bWall) { m_bWall = bWall; }
	_bool			Is_Wall() { return m_bWall; }


	void			Set_PlayerToWall(_float fTimeDelta);
	void			Set_PlayerUp(_float fTimeDelta);

	void			Body_Collision(CGameObject* pObject);
	void			Hit_Attack(CCollider* pCollider);

	void			Add_Hit();

	void			DisAppear_FootTrail();
	void			Appear_FootTrail();


	void			Set_HitEffect(_bool bHitEffect) { m_bHitEffect = bHitEffect; }


	void			Set_EnemyBodyHit(_bool bEnemyBodyHit) { m_bEnemyBodyHit = bEnemyBodyHit; }
	_bool			Is_EnemyBodyHit() { return m_bEnemyBodyHit; }


	void			Effect_Teleport();


	const SKILLINFO& Get_SkillInfo(_uint iSkill) { return m_SkillInfo[iSkill]; }
	_bool				Get_SkillReady(_uint iSkill) { return m_SkillInfo[iSkill].m_bReady; }
	void				Set_SkillReady(_uint iSkill, _bool bReady) { m_SkillInfo[iSkill].m_bReady = bReady; }


	

	const wstring& Get_NickName() { return m_szNickName; }
	void			Set_NickName(const wstring& szNickName);


	void			Set_DefaultUp(_bool bUp) { m_bSetDefaultUp = bUp; }


	CWireTrail*				Get_WireTrail() { return m_pWireTrail; }
	virtual HRESULT			Ready_WireTrail();


	const wstring& Get_VoiceSoundKey() { return m_VoiceSoundKey; }
	void			Set_VoiceSoundKey(const wstring& VoiceSound) { m_VoiceSoundKey = VoiceSound; }
	void			Set_VoiceSoundKey(const wstring& VoiceSound, _float fDelay) { m_VoiceSoundKey = VoiceSound; m_fVoiceSoundDelay = fDelay; }
	_bool			Stop_VoiceSound();

	void			Reset_SlowMotion() { m_iSlowMotionCount = 0; Set_SlowMotion(false); }
protected:
	virtual HRESULT Ready_Components();
	
	void					CullingObject();
	void					Update_Skill(SKILLINFO& tSkill, _float fTimeDelta);
	HRESULT			Ready_UI_OtherPlayer();

protected:
	class CCamera_Player*			m_pCamera = nullptr;
	class CUI_Hits*					m_pHitUI = nullptr;
	

	_float							m_fMoveSpeed = 0.0f;
	_float							m_fAttackMoveSpeed = 0.0f;
	_float							m_fAnimationSpeed = 1.0f;

	unordered_map<wstring, _uint>	m_BoneIndex;
	_bool							m_bDoubleJump = true;


	_bool							m_bWall = false;
	
	_bool							m_bHitEffect = false;


	std::future<HRESULT>			m_PlayAnimation;


	CFootTrail*						m_pFootTrail[2];
	_uint							m_iFootBoneIndex[2];



	_bool							m_bEnemyBodyHit = false;
	_bool							m_bSetDefaultUp = true;

	atomic<_int>					m_iSlowMotionCount = 0;


	CWireTrail*						m_pWireTrail = nullptr;
protected: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;


	//Culling
	BoundingSphere m_tCullingSphere;


	vector<SKILLINFO> m_SkillInfo;


	wstring m_szNickName;
	wstring m_VoiceSoundKey;
	_float m_fVoiceSoundDelay;
public:
	virtual void Free();


};

END

