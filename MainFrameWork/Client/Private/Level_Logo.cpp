#include "stdafx.h"
#include "..\Public\Level_Logo.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "ServerSession.h"
#include "Service.h"
#include "ThreadManager.h"
#include "ServerSessionManager.h"
#include "UI.h"
#include "UI_NickName.h"

CLevel_Logo::CLevel_Logo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;


	m_bConnect = false;


	m_eState = LOGOSTATE::LOGO;

	/*while (CServerSessionManager::GetInstance()->Is_Connected() == false)
	{
	}*/


	if (FAILED(Ready_SoundTrack()))
		return E_FAIL;

	CGameInstance::GetInstance()->PlayBGM(L"LoggoSound.wav", CHANNELID::CHANNEL_BGM, g_fVolume * 0.2f);

	return S_OK;
}

HRESULT CLevel_Logo::Tick(_float fTimeDelta)
{
	switch (m_eState)
	{
	case LOGOSTATE::LOGO:
		Tick_LOGO(fTimeDelta);
		break;
	case LOGOSTATE::NICKNAME:
		Tick_NICKNAME(fTimeDelta);
		break;
	case LOGOSTATE::ENTERGAME:
		Tick_ENTERGAME(fTimeDelta);
		break;
	}

	return S_OK;
}

HRESULT CLevel_Logo::LateTick(_float fTimeDelta)
{
	


	return S_OK;
}

HRESULT CLevel_Logo::Exit()
{
	CGameInstance::GetInstance()->StopSoundAll();


	return S_OK;
}

HRESULT CLevel_Logo::Ready_SoundTrack()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	pGameInstance->Add_SoundTrack(L"MoveSound", L"Move_1.wav");
	pGameInstance->Add_SoundTrack(L"MoveSound", L"Move_2.wav");
	pGameInstance->Add_SoundTrack(L"MoveSound", L"Move_3.wav");

	pGameInstance->Add_SoundTrack(L"HitSound", L"Hit_1.wav");
	pGameInstance->Add_SoundTrack(L"HitSound", L"Hit_2.wav");
	pGameInstance->Add_SoundTrack(L"HitSound", L"Hit_3.wav");


	pGameInstance->Add_SoundTrack(L"BombSound", L"Bomb_1.wav");
	pGameInstance->Add_SoundTrack(L"BombSound", L"Bomb_2.wav");


	pGameInstance->Add_SoundTrack(L"Naruto_RasenganStart", L"Naruto_RasenganStart.wav");
	pGameInstance->Add_SoundTrack(L"Naruto_RasenganStart", L"Naruto_RasenganStart_1.wav");
	pGameInstance->Add_SoundTrack(L"Naruto_RasenganStart", L"Naruto_RasenganStart_2.wav");


	pGameInstance->Add_SoundTrack(L"Naruto_NormalAttack", L"Naruto_NormalAttack_1.wav");
	pGameInstance->Add_SoundTrack(L"Naruto_NormalAttack", L"Naruto_NormalAttack_2.wav");
	pGameInstance->Add_SoundTrack(L"Naruto_NormalAttack", L"Naruto_NormalAttack_3.wav");

	pGameInstance->Add_SoundTrack(L"Naruto_LargeAttack", L"Naruto_LargeAttack.wav");
	pGameInstance->Add_SoundTrack(L"Naruto_LargeAttack", L"Naruto_LargeAttack_1.wav");

	pGameInstance->Add_SoundTrack(L"Naruto_Jump", L"Naruto_Jump.wav");
	pGameInstance->Add_SoundTrack(L"Naruto_Jump", L"Naruto_Jump_1.wav");


	pGameInstance->Add_SoundTrack(L"Naruto_SpinBlowUp", L"Naruto_SpinBlowUp.wav");
	pGameInstance->Add_SoundTrack(L"Naruto_SpinBlowUp", L"Naruto_SpinBlowUp_1.wav");


	pGameInstance->Add_SoundTrack(L"Naruto_HitMiddle", L"Naruto_HitMiddle.wav");
	pGameInstance->Add_SoundTrack(L"Naruto_HitMiddle", L"Naruto_HitMiddle_1.wav");
	pGameInstance->Add_SoundTrack(L"Naruto_HitMiddle", L"Naruto_HitMiddle_2.wav");
	pGameInstance->Add_SoundTrack(L"Naruto_HitMiddle", L"Naruto_HitMiddle_3.wav");


	pGameInstance->Add_SoundTrack(L"Sasuke_ChidoriStart", L"Sasuke_ChidoriStart.wav");
	pGameInstance->Add_SoundTrack(L"Sasuke_ChidoriStart", L"Sasuke_ChidoriStart_1.wav");


	pGameInstance->Add_SoundTrack(L"Sasuke_Jump", L"Sasuke_Jump.wav");
	pGameInstance->Add_SoundTrack(L"Sasuke_Jump", L"Sasuke_Jump_1.wav");


	pGameInstance->Add_SoundTrack(L"Sasuke_NormalAttack", L"Sasuke_NormalAttack.wav");
	pGameInstance->Add_SoundTrack(L"Sasuke_NormalAttack", L"Sasuke_NormalAttack_1.wav");
	pGameInstance->Add_SoundTrack(L"Sasuke_NormalAttack", L"Sasuke_NormalAttack_2.wav");

	pGameInstance->Add_SoundTrack(L"Sasuke_LargeAttack", L"Sasuke_LargeAttack.wav");
	pGameInstance->Add_SoundTrack(L"Sasuke_LargeAttack", L"Sasuke_LargeAttack_1.wav");

	pGameInstance->Add_SoundTrack(L"Sasuke_SpinBlowUp", L"Sasuke_SpinBlowUp.wav");
	pGameInstance->Add_SoundTrack(L"Sasuke_SpinBlowUp", L"Sasuke_SpinBlowUp_1.wav");

	pGameInstance->Add_SoundTrack(L"Sasuke_HitMiddle", L"Sasuke_HitMiddle.wav");
	pGameInstance->Add_SoundTrack(L"Sasuke_HitMiddle", L"Sasuke_HitMiddle_1.wav");
	pGameInstance->Add_SoundTrack(L"Sasuke_HitMiddle", L"Sasuke_HitMiddle_2.wav");
	pGameInstance->Add_SoundTrack(L"Sasuke_HitMiddle", L"Sasuke_HitMiddle_3.wav");

	pGameInstance->Add_SoundTrack(L"Deidara_HitMiddle", L"Deidara_HitMiddle.wav");
	pGameInstance->Add_SoundTrack(L"Deidara_HitMiddle", L"Deidara_HitMiddle_1.wav");

	pGameInstance->Add_SoundTrack(L"Deidara_LargeAttack", L"Deidara_LargeAttack.wav");
	pGameInstance->Add_SoundTrack(L"Deidara_LargeAttack", L"Deidara_LargeAttack_1.wav");


	pGameInstance->Add_SoundTrack(L"Deidara_NormalAttack", L"Deidara_NormalAttack.wav");
	pGameInstance->Add_SoundTrack(L"Deidara_NormalAttack", L"Deidara_NormalAttack_1.wav");

	pGameInstance->Add_SoundTrack(L"Deidara_Skill", L"Deidara_Skill.wav");
	pGameInstance->Add_SoundTrack(L"Deidara_Skill", L"Deidara_Skill_1.wav");
	pGameInstance->Add_SoundTrack(L"Deidara_Skill", L"Deidara_Skill_2.wav");

	pGameInstance->Add_SoundTrack(L"Deidara_SpinBlowUp", L"Deidara_SpinBlowUp.wav");
	pGameInstance->Add_SoundTrack(L"Deidara_SpinBlowUp", L"Deidara_SpinBlowUp_1.wav");


	pGameInstance->Add_SoundTrack(L"Madara_FireBall", L"Madara_FireBall.wav");
	pGameInstance->Add_SoundTrack(L"Madara_FireBall", L"Madara_FireBall_1.wav");

	pGameInstance->Add_SoundTrack(L"Madara_HitMiddle", L"Madara_HitMiddle.wav");
	pGameInstance->Add_SoundTrack(L"Madara_HitMiddle", L"Madara_HitMiddle_1.wav");
	pGameInstance->Add_SoundTrack(L"Madara_HitMiddle", L"Madara_HitMiddle_2.wav");

	pGameInstance->Add_SoundTrack(L"Madara_LargeAttack", L"Madara_LargeAttack.wav");
	pGameInstance->Add_SoundTrack(L"Madara_LargeAttack", L"Madara_LargeAttack_1.wav");
	pGameInstance->Add_SoundTrack(L"Madara_LargeAttack", L"Madara_LargeAttack_2.wav");


	pGameInstance->Add_SoundTrack(L"Madara_Meteor", L"Madara_Meteor.wav");
	pGameInstance->Add_SoundTrack(L"Madara_Meteor", L"Madara_Meteor_1.wav");

	pGameInstance->Add_SoundTrack(L"Madara_NormalAttack", L"Madara_NormalAttack.wav");
	pGameInstance->Add_SoundTrack(L"Madara_NormalAttack", L"Madara_NormalAttack_1.wav");
	pGameInstance->Add_SoundTrack(L"Madara_NormalAttack", L"Madara_NormalAttack_2.wav");
	pGameInstance->Add_SoundTrack(L"Madara_NormalAttack", L"Madara_NormalAttack_3.wav");
	pGameInstance->Add_SoundTrack(L"Madara_NormalAttack", L"Madara_NormalAttack_4.wav");

	pGameInstance->Add_SoundTrack(L"Madara_SpinBlowUp", L"Madara_SpinBlowUp.wav");
	pGameInstance->Add_SoundTrack(L"Madara_SpinBlowUp", L"Madara_SpinBlowUp_1.wav");




	Safe_Release(pGameInstance);
	return S_OK;
}

void CLevel_Logo::Tick_LOGO(_float fTimeDelta)
{
	_bool bKey = false;

	if (KEY_TAP(KEY::ENTER))
	{
		m_pNickNameDialog->Appear();
		m_eState = LOGOSTATE::NICKNAME;
	}
}

void CLevel_Logo::Tick_NICKNAME(_float fTimeDelta)
{
	wstring szNickName = CServerSessionManager::GetInstance()->Get_NickName();
	if (!szNickName.empty())
	{
		m_eState = LOGOSTATE::ENTERGAME;
	}
}

void CLevel_Logo::Tick_ENTERGAME(_float fTimeDelta)
{
	if (m_bConnect == false)
	{
		m_bConnect = true;


		SetWindowText(g_hWnd, TEXT("서버에 접속중입니다."));

		CClientPacketHandler::Init();

		this_thread::sleep_for(1s);

		ClientServiceRef service = make_shared<ClientService>(
			NetAddress(SERVER_IP, SERVER_PORT),
			make_shared<IocpCore>(),
			make_shared<CServerSession>, // TODO : SessionManager 등
			1);

		ASSERT_CRASH(service->Start());

		for (int32 i = 0; i < 3; i++)
		{
			ThreadManager::GetInstance()->Launch([=]()
				{
					while (true)
					{
						service->GetIocpCore()->Dispatch();
					}
				});
		}

	}
}

HRESULT CLevel_Logo::Ready_Layer_BackGround()
{
	/* 원형객체를 복제하여 사본객체를 생성하고 레이어에 추가한다. */
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOGO, _uint(LAYER_TYPE::LAYER_BACKGROUND), TEXT("Prototype_GameObject_BackGround_MainLogo")))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	CGameObject* pObject = pGameInstance->Add_GameObject(LEVEL_LOGO, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_NickNameDialog"));
	if (pObject == nullptr)
		return E_FAIL;

	m_pNickNameDialog = dynamic_cast<CUI_NickName*>(pObject);


	if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOGO, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_NickNameInput")))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Logo*	pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
 	__super::Free();


}
