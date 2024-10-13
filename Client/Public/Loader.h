#pragma once

/* 다음 레벨에 대한 자원을 준비한다. */
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final : public CBase
{
private:
								CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual						~CLoader() = default;


private:
	ID3D11Device*				m_pDevice  = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	CGameInstance*				m_pGameInstance = { nullptr };
private:
	HANDLE						m_hThread{};
	CRITICAL_SECTION			m_CriticalSection{};
	LEVELID						m_eNextLevelID = {};
private:
	_wstring					m_strLoadingText{};
	_bool						m_bFinished{};

#ifdef _DEBUG
public:
	void Output_LoadingState();
#endif

private:
	HRESULT						Loading_For_MenuLevel();
	HRESULT						Loading_For_Stage1Level();
	HRESULT						Loading_For_Stage2Level();

public:
	const _bool&				Finished() { return m_bFinished; }
	HRESULT						Loading();
	HRESULT						Initialize(LEVELID eNextLevelID);
public:
	static CLoader*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID);
	virtual void				Free() override;
};

END