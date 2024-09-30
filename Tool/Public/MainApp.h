#pragma once

#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Tool)
class CMainApp final : public CBase
{
private:
						 CMainApp();
	virtual				 ~CMainApp() = default;

public:
	HRESULT				 Initialize();
	void				 Update(_float fTimeDelta);
	void				 Render();

private:
	CGameInstance*		 m_pGameInstance = { nullptr };

	ID3D11Device*		 m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	HRESULT Open_Level(LEVELID eLevelID);
	HRESULT Ready_Prototype_For_Component();
	HRESULT Ready_Prototype_For_GameObject();
public:
	static CMainApp*	 Create();
	virtual void		 Free() override;
};

END;

