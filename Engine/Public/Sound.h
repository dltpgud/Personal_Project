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
    void Play_Sound(_tchar* pSoundKey, FMOD::Channel** ppChannel, _float fVolume, _bool bLoop = false);
    void PlayBGM(FMOD::Channel** ppChannel, _tchar* pSoundKey, _float fVolume);
    void StopSound(FMOD::Channel** ppChannel);
	void StopAll();
    void SetChannelVolume(FMOD::Channel** ppChannel, _float fDis, _vector vLength);

public:
	void LoadSoundFile(const _char* soundFile);


private:	
	USE_LOCK;

	map<TCHAR*, Sound*> m_mapSound;
    ChannelGroup* m_pChannelGroupBGM{};
    ChannelGroup* m_pChannelGroupSE{};
	System* m_pSystem = nullptr;

public:
    static CSound* Create();
	virtual void Free() override;
};

END