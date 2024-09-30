#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLEVEL_MENU final : public CLevel
{
private:
			CLEVEL_MENU(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLEVEL_MENU() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer(const _uint& pLayerTag);
public:
	static CLEVEL_MENU* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void		Free() override;
};

END