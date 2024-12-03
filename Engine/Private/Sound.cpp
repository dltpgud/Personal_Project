
#include "Sound.h"

CSound::CSound() 
{
	ZeroMemory(&m_pChannelArr, sizeof(Channel*) * SOUND_EFFECT * SOUND_BGM * SOUND_BGM2 * MAXCHANNEL);
	m_pSystem = nullptr;
}


HRESULT CSound::Initialize()
{
	// 사운드를 담당하는 대표객체를 생성하는 함수
	result = System_Create(&m_pSystem, FMOD_VERSION);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	result = m_pSystem->init(512, FMOD_INIT_NORMAL, NULL);

	//FMOD::System* system;
	//FMOD::ChannelGroup* channelGroup = nullptr;
	//system->createChannelGroup("MyChannelGroup", &channelGroup);


	return S_OK;
}


void CSound::Play_Sound(_tchar* pSoundKey, CHANNELID eID, _float fVolume)
{
	map<TCHAR*, Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->_bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	_bool* bPlay = FALSE;

	//if (m_pChannelArr[eID]->isPlaying(bPlay))
	//{
	//	cout << "in" << endl;
	result = m_pSystem->playSound(iter->second, nullptr, false, &m_pChannelArr[eID]);
	//}

	m_pChannelArr[eID]->setVolume(fVolume);

	m_pSystem->update();
}

void CSound::PlayBGM(CHANNELID eID, _tchar* pSoundKey, _float fVolume)
{
	map<TCHAR*, Sound*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->_bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	result = m_pSystem->playSound(iter->second, nullptr, false, &m_pChannelArr[eID]);
	m_pChannelArr[eID]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[eID]->setVolume(fVolume);
}





void CSound::StopSound(CHANNELID eID)
{
	m_pChannelArr[eID]->stop();
}

void CSound::StopAll()
{
	for (size_t i = 0; i < MAXCHANNEL; ++i)
		m_pChannelArr[i]->stop();
}

void CSound::SetChannelVolume(CHANNELID eID, _float fVolume)
{
	m_pChannelArr[eID]->setVolume(fVolume);
	m_pSystem->update();
}

void CSound::LoadSoundFile(const _char* soundFile)
{
	_char szCurPath[128] = "../../Client/Bin/Resources/Sound/";	 // 상대 경로
	_char szFullPath[128] = "";

	strcpy_s(szFullPath, szCurPath);

	_char szfull[128]{};

	strcpy_s(szfull, soundFile);
	// "../Sound/" + "Success.wav"

	strcat_s(szFullPath, szfull);
	// "../Sound/Success.wav"

	Sound* pSound = nullptr;
	FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_DEFAULT, 0, &pSound);

	if (eRes == FMOD_OK)
	{
		_int iLength = (_int)(strlen(szfull) + 1);

		TCHAR* pSoundKey = new TCHAR[iLength];
		ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

		// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
		MultiByteToWideChar(CP_ACP, 0, szfull, iLength, pSoundKey, iLength);

		m_mapSound.emplace(pSoundKey, pSound);
	}

	m_pSystem->update();
}



CSound* CSound::Create()
{
	CSound* pInstance = new CSound();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CSound");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound::Free()
{
	__super::Free();

	for (auto& Mypair : m_mapSound)
	{
		delete[]Mypair.first;
		Mypair.second->release();
	}
	m_mapSound.clear();

	channelGroup->release();
	m_pSystem->release();
}
