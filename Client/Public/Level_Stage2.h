 #pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Stage2 final : public CLevel
{
private:
	CLevel_Stage2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Stage2() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private: 
	HRESULT Ready_Layer_Player();
    HRESULT Ready_Layer_Monster(const _wstring& pLayerTag);
    HRESULT Ready_Layer_Map(const _wstring& pLayerTag);
    HRESULT Ready_Layer_NPC(const _wstring& pLayerTag);
	HRESULT Ready_Find_cell();
	HRESULT Ready_Light();
    HRESULT Ready_UI();

    private:
    class CFade* m_pFade = {nullptr};

public:
	static CLevel_Stage2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END