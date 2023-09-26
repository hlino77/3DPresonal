#pragma once
#include "Server_Defines.h"
#include "Level.h"

class CLevel_Ready_Server final : public CLevel
{
private:
	CLevel_Ready_Server();
	virtual ~CLevel_Ready_Server() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;

private:
	HRESULT Ready_Layer_BackGround();

public:
	static class CLevel_Ready_Server* Create();
	virtual void Free() override;
};

