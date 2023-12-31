#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"
#include "AsFileUtils.h"
#include "AsUtils.h"
#include "Player_Naruto.h"
#include "Camera_Free.h"
#include "StaticModel.h"
#include "ServerSession.h"
#include "Service.h"
#include "ThreadManager.h"
#include "ServerSessionManager.h"
#include "Camera_Player.h"
#include "Player_Sasuke.h"
#include "Monster_WhiteZetsu.h"
#include "MainLogo.h"
#include "UI_NickName.h"
#include "UI_NickNameInput.h"
#include "BackGround_Lobby.h"
#include "UI_TitleCloud.h"
#include "UI_Title.h"
#include "UI_TitleSymbol.h"
#include "UI_PlayerWindow.h"
#include "UI_PlayerWindowTitle.h"
#include "UI_PlayerInfo.h"
#include "UI_CharacterSelect.h"
#include "Player_Lobby.h"
#include "ColMesh.h"
#include "Boss_Deidara.h"
#include "Monster_C2Dragon.h"
#include "Skill_Hiryu.h"
#include "Skill_TwinBird.h"
#include "UI_BattleStart.h"
#include "UI_Hits.h"
#include "UI_SP_Base.h"
#include "UI_HP_Base.h"
#include "LineCircle.h"
#include "FootTrail.h"
#include "MonsterSpawner.h"
#include "BossSpawner.h"
#include "Boss_Madara.h"
#include "RasenganCircle.h"
#include "Skill_Rasengun.h"
#include "SmokeRing.h"
#include "ChidoriCenter.h"
#include "Skill_Chidori.h"
#include "Lightning_A_01.h"
#include "Lightning_A_02.h"
#include "Lightning_B_01.h"
#include "Lightning_B_02.h"
#include "Lightning_B_03.h"
#include "Lightning_C_01.h"
#include "Lightning_Chidori.h"
#include "LightningTrail.h"
#include "Rasengan_StartSmoke.h"
#include "Rasengan_RunSmoke.h"
#include "Skill_Meteor.h"
#include "Meteor_HemiSphere.h"
#include "Meteor_Bottom.h"
#include "Meteor_Smoke.h"
#include "Smoke_09.h"
#include "Smoke_19.h"
#include "Meteor_Cylinder.h"
#include "Monster_Susanoo_Madara.h"
#include "FireBall.h"
#include "FireBallPlane.h"
#include "FireBallTrail.h"
#include "Fire.h"
#include "Smoke_24.h"
#include "ExplosionRing.h"
#include "Skill_FireBall.h"
#include "RasenSyurikenBlade.h"
#include "RasenSyurikenBladeBlur.h"
#include "RasenSyurikenBladeLine.h"
#include "Skill_RasenSyuriken.h"
#include "RasenSyurikenCenter.h"
#include "RasenSyurikenSphere.h"
#include "SmokeRing_Syuriken.h"
#include "Teleport.h"
#include "Explosion.h"
#include "Skill_MadaraFireBall.h"
#include "SkyBoxDay.h"
#include "SkyBoxNight.h"
#include "UI_Skill.h"
#include "UI_HP_Monster.h"
#include "UI_HP_Boss.h"
#include "UI_BossIcon.h"
#include "UI_MatchingLobbyTitle.h"
#include "UI_OtherPlayer.h"
#include "WireTrail.h"


CLoader::CLoader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

// unsigned(__stdcall* _beginthreadex_proc_type)(void*);

_uint APIENTRY ThreadEntry(void* pArg)
{
	
	CoInitializeEx(nullptr, 0);

	/* 새롭게 생성된 스레드가 일한다. */
	CLoader*		pLoader = (CLoader*)pArg;

	pLoader->Loading();

	return 0;
}

HRESULT CLoader::Initialize(LEVELID eNextLevel)
{
	InitializeCriticalSection(&m_Critical_Section);

	m_eNextLevel = eNextLevel;

	/* 새로운 스레드를 만들자. */
	/* 스레드 : 내 코드를 읽어주는 하나의 흐름? */
	/* 3 : 생성한 스레드가 호출해야하는 함수의 주소 */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntry, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

_int CLoader::Loading()
{
	EnterCriticalSection(&m_Critical_Section);

	HRESULT		hr = 0;

	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Level_Logo();
		break;
	case LEVEL_LOBBY:
		hr = Loading_For_Level_Lobby();
		break;
	case LEVEL_ARENA:
		hr = Loading_For_Level_Arena();
		break;
	case LEVEL_KONOHA:
		hr = Loading_For_Level_Konoha();
		break;
	}

	if (FAILED(hr))
		return -1;	

	LeaveCriticalSection(&m_Critical_Section);

	return 0;	
}

HRESULT CLoader::Loading_For_Level_Logo()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Texture */
	m_strLoading = TEXT("텍스쳐를 로딩 중 입니다.");

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_NickNameDialog"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/NickNameDialog.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_NickNameInput"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/NickNameInput.png"))))
		return E_FAIL;

	/* For.Shader */
	m_strLoading = TEXT("셰이더를 로딩 중 입니다.");

	/* For.GameObject */
	m_strLoading = TEXT("객체원형을 로딩 중 입니다.");

	///* For.Prototype_GameObject_BackGround */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_NickNameDialog"), CUI_NickName::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_NickNameInput"), CUI_NickNameInput::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	Safe_Release(pGameInstance);

	m_strLoading = TEXT("로딩 끝.");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Level_Arena()
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Texture */
	m_strLoading = TEXT("텍스쳐를 로딩 중 입니다.");
	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BattleStart"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/BattleStart.png"))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HitCount"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/HitCount_%d.png", 10))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Hit"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Hit_%d.png", 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_SP_Base"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/SP_Base.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HP_Base"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/HP_Base.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HP_Back"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/HP_Back.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HP_Gauge"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/HP_Gauge.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HP_GaugeMask"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/HP_GaugeMask.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HP_Damage"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/HP_Damage.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Skill_Base"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Skill_Base.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Skill_Mask"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Skill_Mask.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Skill_Icon"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Skill_%d.png", 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Skill_ShadowMask"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Skill_ShadowMask.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BossIcon"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/BossIcon_%d.png", 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Rank"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Rank.png", 2))))
		return E_FAIL;



	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Circle"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Circle.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_RasenganCircle"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/RasenganCircle.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Ring"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Ring.png"))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_SmokeRing"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/SmokeRing.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_ChidoriCenter"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/ChidoriCenter.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Lightning"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Lightning.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_LineSmoke"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/LineSmoke.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Meteor_HemiSphere"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Meteor_HemiSphere.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Meteor_Bottom"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Meteor_Bottom.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Meteor_Smoke"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Meteor_Smoke.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Smoke_09"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Smoke_09.png"))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Smoke_19"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Smoke_19.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Default"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Default.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_FireBallOut"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/FireBallOut.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_FirePlane"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/FirePlane.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_FireDisolve"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/FireDisolve_%d.png", 2))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_FireBallTrail"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/FireBallTrail.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Fire"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Fire.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Fire_Mask"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Fire_Mask.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Smoke_24"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Smoke_24.png"))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_ExplosionRing"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/ExplosionRing.png"))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_BladeBlur"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/BladeBlur.png"))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_NoiseLine_BC"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/NoiseLine_BC.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_NoiseLine_M"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/NoiseLine_M.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_SphereNoise"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/SphereNoise.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_SphereDisolve"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/SphereDisolve.png"))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_TeleportLine"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/TeleportLine.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Explosion_BC"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Explosion_BC.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Explosion_M"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/Explosion_M.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_ExplosionDisolve"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effects/ExplosionDisolve.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkyBoxDay"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/SkyBox/SkyBoxDay.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkyBoxNight"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/SkyBox/SkyBoxNight.png"))))
		return E_FAIL;




	/* For.Mesh */
	m_strLoading = TEXT("메시를 로딩 중 입니다.");
	

	/* For.Shader */
	m_strLoading = TEXT("셰이더를 로딩 중 입니다.");

	m_strLoading = TEXT("객체 원형을 로딩 중 입니다.");

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Player"),
		CCamera_Player::Create(m_pDevice, m_pContext, L"Player_Camera"))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Naruto"),
		CPlayer_Naruto::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Sasuke"),
		CPlayer_Sasuke::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_WhiteZetsu"),
		CMonster_WhiteZetsu::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Deidara"),
		CBoss_Deidara::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_C2Dragon"),
		CMonster_C2Dragon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Susanoo_Madara"),
		CMonster_Susanoo_Madara::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StaticModel"),
		CStaticModel::Create(m_pDevice, m_pContext, PROP))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ColMesh"),
		CColMesh::Create(m_pDevice, m_pContext, COLMESH))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Hiryu"),
		CSkill_Hiryu::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_TwinBird"),
		CSkill_TwinBird::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterSpawner"),
		CMonsterSpawner::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossSpawner"),
		CBossSpawner::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RasenganCircle"),
		CRasenganCircle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Rasengun"),
		CSkill_Rasengun::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Chidori"),
		CSkill_Chidori::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Meteor"),
		CSkill_Meteor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_FireBall"),
		CSkill_FireBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_RasenSyuriken"),
		CSkill_RasenSyuriken::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_MadaraFireBall"),
		CSkill_MadaraFireBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_SmokeRing"),
		CSmokeRing::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_ChidoriCenter"),
		CChidoriCenter::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Lightning_A_01"),
		CLightning_A_01::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Lightning_A_02"),
		CLightning_A_02::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Lightning_B_01"),
		CLightning_B_01::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Lightning_B_02"),
		CLightning_B_02::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Lightning_B_03"),
		CLightning_B_03::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Lightning_C_01"),
		CLightning_C_01::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Lightning_Chidori"),
		CLightning_Chidori::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Rasengan_StartSmoke"),
		CRasengan_StartSmoke::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Rasengan_RunSmoke"),
		CRasengan_RunSmoke::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Meteor_HemiSphere"),
		CMeteor_HemiSphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Meteor_Bottom"),
		CMeteor_Bottom::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Meteor_Smoke"),
		CMeteor_Smoke::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Smoke_09"),
		CSmoke_09::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Smoke_19"),
		CSmoke_19::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Meteor_Cylinder"),
		CMeteor_Cylinder::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_FireBall"),
		CFireBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_FireBallPlane"),
		CFireBallPlane::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_FireBallTrail"),
		CFireBallTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Fire"),
		CFire::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Smoke_24"),
		CSmoke_24::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_ExplosionRing"),
		CExplosionRing::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_RasenSyurikenBlade"),
		CRasenSyurikenBlade::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_RasenSyurikenBladeBlur"),
		CRasenSyurikenBladeBlur::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_RasenSyurikenBladeLine"),
		CRasenSyurikenBladeLine::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_RasenSyurikenCenter"),
		CRasenSyurikenCenter::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_RasenSyurikenSphere"),
		CRasenSyurikenSphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_SmokeRing_Syuriken"),
		CSmokeRing_Syuriken::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Teleport"),
		CTeleport::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Explosion"),
		CExplosion::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BattleStart"), CUI_BattleStart::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Hits"), CUI_Hits::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_SP_Base"), CUI_SP_Base::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_HP_Base"), CUI_HP_Base::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Skill"), CUI_Skill::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_HP_Monster"), CUI_HP_Monster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_HP_Boss"), CUI_HP_Boss::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BossIcon"), CUI_BossIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_OtherPlayer"), CUI_OtherPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_LineCircle"),
		CLineCircle::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_FootTrail"),
		CFootTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_LightningTrail"),
		CLightningTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_WireTrail"),
		CWireTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkyBoxDay"),
		CSkyBoxDay::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkyBoxNight"),
		CSkyBoxNight::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	m_strLoading = TEXT("모델을 로딩 중 입니다.");
	Loading_Model_For_Level_Arena();



	Load_MapData(LEVEL_ARENA, L"../Bin/Resources/MapData/Arena.data");
	Load_ColMesh(LEVEL_ARENA, L"../Bin/Resources/ColMeshData/Arena.data");


	m_strLoading = TEXT("로딩 끝.");
	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_For_Level_Lobby()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Texture */
	m_strLoading = TEXT("텍스쳐를 로딩 중 입니다.");


	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext, L"Free_Camera"))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BackGround_Lobby"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/BackGround/BackGround_Lobby.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TitleCloud"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/TitleCloud_%d.png", 11))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Title"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Title.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TitleSymbol"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/TitleSymbol.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Window"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Window.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_WindowTitle"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/WindowTitle.png"))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerInfo"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/PlayerInfo.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_OtherPlayerInfo"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/OtherPlayerInfo.png"))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Character"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/Character_%d.png", 5))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ReadyMark"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/ReadyMark.png"))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MatchingLobbyTitle"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/UI/MatchingLobbyTitle.png"))))
		return E_FAIL;


	/* For.Shader */
	m_strLoading = TEXT("셰이더를 로딩 중 입니다.");

	/* For.GameObject */
	m_strLoading = TEXT("객체원형을 로딩 중 입니다.");


	Matrix		PivotMatrix = XMMatrixIdentity();

	{
		wstring strFileName = L"Naruto";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOBBY, strComponentName,
			CModel::Create(m_pDevice, m_pContext, strFilePath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}

	{
		wstring strFileName = L"Sasuke";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOBBY, strComponentName,
			CModel::Create(m_pDevice, m_pContext, strFilePath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}

	///* For.Prototype_GameObject_BackGround */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround_Lobby"), CBackGround_Lobby::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_TitleCloud"), CUI_TitleCloud::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Title"), CUI_Title::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_TitleSymbol"), CUI_TitleSymbol::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_PlayerWindow"), CUI_PlayerWindow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_PlayerWindowTitle"), CUI_PlayerWindowTitle::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_PlayerInfo"), CUI_PlayerInfo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_CharacterSelect"), CUI_CharacterSelect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MatchingLobbyTitle"), CUI_MatchingLobbyTitle::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Lobby"),
		CPlayer_Lobby::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	

	Safe_Release(pGameInstance);

	m_strLoading = TEXT("로딩 끝.");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Level_Konoha()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Texture */
	m_strLoading = TEXT("텍스쳐를 로딩 중 입니다.");


	/* For.Mesh */
	m_strLoading = TEXT("메시를 로딩 중 입니다.");


	/* For.Shader */
	m_strLoading = TEXT("셰이더를 로딩 중 입니다.");

	m_strLoading = TEXT("객체 원형을 로딩 중 입니다.");

	/* For.Prototype_GameObject_Camera_Free */


	m_strLoading = TEXT("모델을 로딩 중 입니다.");
	Loading_Model_For_Level_Konoha();


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Madara"),
		CBoss_Madara::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	Load_MapData(LEVEL_KONOHA, L"../Bin/Resources/MapData/Kono.data");
	Load_ColMesh(LEVEL_KONOHA, L"../Bin/Resources/ColMeshData/Kono.data");


	m_strLoading = TEXT("로딩 끝.");
	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Load_MapData(LEVELID eLevel, const wstring& szFilePath)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	shared_ptr<CAsFileUtils> file = make_shared<CAsFileUtils>();
	file->Open(szFilePath, FileMode::Read);

	Matrix		PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));


	_uint iSize = file->Read<_uint>();

	for (_uint i = 0; i < iSize; ++i)
	{
		wstring szModelName = CAsUtils::ToWString(file->Read<string>());
		Matrix	matWorld = file->Read<Matrix>();

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";

		wstring strComponentName = L"Prototype_Component_Model_" + szModelName;

		if (FAILED(pGameInstance->Check_Prototype(eLevel, strComponentName)))
		{
			_uint iColCount = file->Read<_uint>();
			for (_uint i = 0; i < iColCount; ++i)
			{
				Vec3 vOffset = file->Read<Vec3>();
				_float fRadius = file->Read<_float>();


				if (file->Read<_bool>())
				{
					Vec3 vOffset = file->Read<Vec3>();
					Vec3 vScale = file->Read<Vec3>();
					Quaternion vQuat = file->Read<Quaternion>();
				}
			}
			continue;
		}
			


		if (FAILED(pGameInstance->Add_Prototype(eLevel, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, szModelName, true, false, PivotMatrix))))
			return E_FAIL;


		_uint iColCount = file->Read<_uint>();

		for (_uint i = 0; i < iColCount; ++i)
		{
			Vec3 vOffset = file->Read<Vec3>();
			_float fRadius = file->Read<_float>();


			if (file->Read<_bool>())
			{
				Vec3 vOffset = file->Read<Vec3>();
				Vec3 vScale = file->Read<Vec3>();
				Quaternion vQuat = file->Read<Quaternion>();
			}
		}
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLoader::Load_ColMesh(LEVELID eLevel, const wstring& szFilePath)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	shared_ptr<CAsFileUtils> file = make_shared<CAsFileUtils>();
	file->Open(szFilePath, FileMode::Read);

	Matrix		PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));


	_uint iSize = file->Read<_uint>();

	for (_uint i = 0; i < iSize; ++i)
	{
		wstring szModelName = CAsUtils::ToWString(file->Read<string>());
		Matrix	matWorld = file->Read<Matrix>();

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";

		wstring strComponentName = L"Prototype_Component_Model_" + szModelName;

		if (FAILED(pGameInstance->Check_Prototype(eLevel, strComponentName)))
		{
			_uint iColCount = file->Read<_uint>();
			for (_uint i = 0; i < iColCount; ++i)
			{
				Vec3 vOffset = file->Read<Vec3>();
				_float fRadius = file->Read<_float>();


				if (file->Read<_bool>())
				{
					Vec3 vOffset = file->Read<Vec3>();
					Vec3 vScale = file->Read<Vec3>();
					Quaternion vQuat = file->Read<Quaternion>();
				}
			}
			continue;
		}


		if (FAILED(pGameInstance->Add_Prototype(eLevel, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, szModelName, true, true, PivotMatrix))))
			continue;


		_uint iColCount = file->Read<_uint>();

		for (_uint i = 0; i < iColCount; ++i)
		{
			Vec3 vOffset = file->Read<Vec3>();
			_float fRadius = file->Read<_float>();


			if (file->Read<_bool>())
			{
				Vec3 vOffset = file->Read<Vec3>();
				Vec3 vScale = file->Read<Vec3>();
				Quaternion vQuat = file->Read<Quaternion>();
			}
		}
	}

	Safe_Release(pGameInstance);
	return S_OK;
}


HRESULT CLoader::Loading_Model_For_Level_Arena()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Matrix		PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));


	{
		wstring strFileName = L"Naruto";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, strFilePath, strFileName, true , false, PivotMatrix))))
			return E_FAIL;
	}

	{
		wstring strFileName = L"Sasuke";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, strFilePath, strFileName, true , false,  PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"WhiteZetsu";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_ARENA, strComponentName,
			CModel::Create(m_pDevice, m_pContext, strFilePath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"Deidara";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_ARENA, strComponentName,
			CModel::Create(m_pDevice, m_pContext, strFilePath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"C2Dragon";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_ARENA, strComponentName,
			CModel::Create(m_pDevice, m_pContext, strFilePath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"Hiryu";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_ARENA, strComponentName,
			CModel::Create(m_pDevice, m_pContext, strFilePath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}

	{
		wstring strFileName = L"TwinBirds";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_ARENA, strComponentName,
			CModel::Create(m_pDevice, m_pContext, strFilePath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}

	{
		wstring strFileName = L"Susanoo_Madara";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, strFilePath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"AreaCircle";

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"SmokeRing";

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"Lightning_A_01";

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}

	{
		wstring strFileName = L"Lightning_A_02";

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}

	{
		wstring strFileName = L"Lightning_B_01";

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"Lightning_B_02";

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"Lightning_B_03";

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}



	{
		wstring strFileName = L"Lightning_C_01";

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"Lightning_Chidori";

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"RasenganStartSmoke";

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}

	{
		wstring strFileName = L"RasenganRunSmoke";

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"Meteor";

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"Meteor_HemiSphere";

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"Meteor_Bottom";

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"Meteor_Smoke";

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}

	{
		wstring strFileName = L"Meteor_Cylinder";

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"FireBall";

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}

	{
		wstring strFileName = L"FireBallPlane";

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}

	{
		wstring strFileName = L"FireBallTrail";

		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}

	{
		wstring strFileName = L"RasenSyurikenBlade";
		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}

	{
		wstring strFileName = L"RasenSyurikenBladeBlur";
		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"RasenSyurikenBladeLine";
		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}

	{
		wstring strFileName = L"CommonSphere";
		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"SkyBoxDay";
		wstring szModelPath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(m_pDevice, m_pContext, szModelPath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}




	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_Model_For_Level_Konoha()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Matrix		PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));


	{
		wstring strFileName = L"WhiteZetsu";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_KONOHA, strComponentName,
			CModel::Create(m_pDevice, m_pContext, strFilePath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}

	{
		wstring strFileName = L"Madara";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_KONOHA, strComponentName,
			CModel::Create(m_pDevice, m_pContext, strFilePath, strFileName, true, false, PivotMatrix))))
			return E_FAIL;
	}


	



	Safe_Release(pGameInstance);
	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eNextLevel)
{
	CLoader*	pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);	

	DeleteCriticalSection(&m_Critical_Section);

	CloseHandle(m_hThread);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}