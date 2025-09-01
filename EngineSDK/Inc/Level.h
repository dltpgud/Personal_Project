#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
							CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual					~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();
   
    HRESULT Load_to_Next_Map_NonaniObj(const _uint& iLevelIndex, const _wstring& strLayerTag, const _wstring& strProto,
                                           const _tchar* strProtoMapPath, void* Arg = nullptr);
    HRESULT Load_to_Next_Map_Wall(const _uint& iLevelIndex, const _wstring& strLayerTag, const _wstring& strProto,
                                      const _tchar* strProtoMapPath, void* Arg = nullptr);
    HRESULT Load_to_Next_Map_AniOBj(const _uint& iLevelIndex, const _wstring& strLayerTag, const _wstring& strProto,
                                         const _tchar* strProtoMapPath, void* Arg = nullptr);
    HRESULT Load_to_Next_Map_Monster(const _uint& iLevelIndex, const _wstring& strLayerTag,const _wstring& strProto,
                                         const _wstring& strProtoComponet, const _tchar* strProtoMapPath, void* Arg = nullptr);
    HRESULT Load_to_Next_Map_NPC(const _uint& iLevelIndex, const _wstring& strLayerTag, const _wstring& strProto,
                                     const _tchar* strProtoMapPath, void* Arg = nullptr);

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };
	_bool m_bPade{ false };
     FMOD::Channel* m_pChannel = nullptr;

    public:
	virtual void			Free() override;
};

END