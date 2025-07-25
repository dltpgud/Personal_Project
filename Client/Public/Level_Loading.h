#pragma once

#include "Client_Defines.h"
#include "Level.h"


/* 현재 로딩화면을 보여준다. */
/* 다음 레벨에 대한 자원을 준비한다.(CLoader 하청을 맡길거야) */

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
	HRESULT			Ready_Clone_Layer(const _uint& pLayerTag);

private:
	/*서브 스레드 생성에 필요한 변수들*/
	class CLoader*				m_pLoader	   = { nullptr };
	LEVELID						m_eNextLevelID = { LEVEL_END };
	
	_float						 m_fTimeSum{};
	_float                     m_fFinishSum{};
public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID, _bool Page = false);
	virtual void Free() override;
};

END