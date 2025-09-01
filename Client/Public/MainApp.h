#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CMainApp final : public CBase
{
private:
						 CMainApp();
	virtual				 ~CMainApp() = default;

public:
	HRESULT				 Initialize();
	void				 Update(_float fTimeDelta);
	void				 Render();
        void Delete();

    private:
	CGameInstance*		 m_pGameInstance = { nullptr };

	ID3D11Device*		 m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	HRESULT Open_Level(LEVELID eLevelID);



private:
	_tchar					m_szFPS[MAX_PATH] = TEXT("");
	_uint					m_iNumRender = { 0 };
	_float					m_fTimeAcc = { 0.f };


public:
	static CMainApp*	 Create();
	virtual void		 Free() override;
};

END;