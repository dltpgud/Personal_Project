#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CLevel_Manager final : public CBase
{
private:
			CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Open_Level(_uint iCurrentLevelID, class CLevel* pNewLevel);
	void	Update(_float fTimeDelta);
	HRESULT Render();
	

	
	_uint Get_iCurrentLevel();
	_bool IsOpenStage() { return OpenLevel; }
	void Set_Open_Bool(_bool NextStage);
private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	class CLevel*			m_pCurrentLevel = { nullptr };
	_uint					m_iCurrentLevelID = {};


	_bool OpenLevel = { false };
public:
	static CLevel_Manager* Create();
	virtual void		   Free() override;
};

END