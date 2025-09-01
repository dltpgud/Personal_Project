#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_StageBoss final : public CLevel
{
private:
	CLevel_StageBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_StageBoss() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private: 
	HRESULT Ready_Layer_Player ();
	HRESULT Ready_Layer_Monster(const _wstring& pLayerTag);
	HRESULT Ready_Layer_Camera (const _wstring& pLayerTag);
	HRESULT Ready_Layer_Map    (const _wstring& pLayerTag);
	HRESULT Ready_Find_cell();
	HRESULT Ready_Light();
    HRESULT Ready_UI();
    HRESULT Load_to_Next_Map_terrain(const _uint& iLevelIndex, const _wstring& strLayerTag,const _wstring& strProto, const _tchar* strProtoMapPath, void* Arg);

private :
	class CSceneCamera* m_pSceneCam = { nullptr};
	_bool m_bstart = false;
public:
	static CLevel_StageBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END