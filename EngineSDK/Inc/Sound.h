#pragma once

#include "Base.h"

BEGIN(Engine)

class CSound :public CBase
{
private:
    CSound();
	virtual ~CSound() = default;

public:
    HRESULT Initialize();

public:
    void Play_Sound(_tchar* pSoundKey, FMOD::Channel** ppChannel, _float fVolume ,_bool bLoop = false);
    void PlayBGM(FMOD::Channel** ppChannel, _tchar* pSoundKey, _float fVolume);
	void StopAll();
    void Set3DListenerAttributes();
    void UpdateSoundPosition(FMOD::Channel* pChannel,class CTransform* pTransform);

public:
    void LoadSoundFile(const _char* soundFile,_bool isBGM = false);


private:	
	//USE_LOCK;
    mutex m_mutex;
	map<TCHAR*, Sound*> m_mapSound;
    ChannelGroup* m_pChannelGroupBGM{};
    ChannelGroup* m_pChannelGroupSE{};
	System* m_pSystem = nullptr;

    class CGameInstance* m_pGameInstance = {nullptr};

public:
    static CSound* Create();
	virtual void Free() override;
};

END