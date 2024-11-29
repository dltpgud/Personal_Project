#pragma once

#include "Base.h"

BEGIN(Engine)

class CSound :public CBase
{
public:
	enum CHANNELID { SOUND_EFFECT, SOUND_EFFECT_BGM, SOUND_EFFECT_BGM2, SOUND_MONSTER_SETB, SOUND_MONSTER_ROLL1, SOUND_MONSTER_FLY,SOUND_MONSTER_ROLL2,SOUND_BGM, SOUND_BGM2, MAXCHANNEL };
private:
	CSound();
	virtual ~CSound() = default;

public:
	HRESULT Initialize();

public:
	void Play_Sound(_tchar* pSoundKey, CHANNELID eID, _float fVolume);
	void PlayBGM(CHANNELID eID, _tchar* pSoundKey, _float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, _float fVolume);

public:
	void LoadSoundFile(const _char* soundFile);


private:	


	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, Sound*> m_mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	Channel* m_pChannelArr[MAXCHANNEL];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	System* m_pSystem = nullptr;

	FMOD_RESULT result{};

	ChannelGroup* channelGroup{};



public:
	static CSound* Create();
	virtual void Free() override;
};

END