#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
public:

private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
    virtual HRESULT Initialize(LEVELID eNextLevelID, _bool Page );
	virtual void	Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
 

private:
	HRESULT			Ready_Clone_Layer();
    HRESULT         Ready_UI();

private:

	class CLoader*				m_pLoader	   = { nullptr };
	LEVELID						m_eNextLevelID = { LEVEL_END };
	
	class CFade*                m_pFade = {nullptr};
	_float						m_fTimeSum{};
public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID, _bool Page = false);
	virtual void Free() override;
};

END