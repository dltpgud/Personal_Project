
#include "Sound.h"

CSound::CSound() : m_pSystem{nullptr}
{
}

HRESULT CSound::Initialize()
{
	System_Create(&m_pSystem, FMOD_VERSION);
	m_pSystem->init(512, FMOD_INIT_NORMAL, NULL);
    m_pSystem->createChannelGroup("BGMChannelGroup", &m_pChannelGroupBGM);
    m_pSystem->createChannelGroup("SEChannelGroup", &m_pChannelGroupSE);
	return S_OK;
}

void CSound::Play_Sound(_tchar* pSoundKey, FMOD::Channel** ppChannel, _float fVolume, _bool bLoop)
{
 map<TCHAR*, Sound*>::iterator iter;
 
 iter = find_if(m_mapSound.begin(), m_mapSound.end(),
 	[&](auto& iter)->_bool
 	{
 		return !lstrcmp(pSoundKey, iter.first);
 	});
 
 if (iter == m_mapSound.end())
 	return;
   
 FMOD::Channel* m_pChannel = nullptr;
   
if (nullptr != ppChannel)
           m_pSystem->playSound(iter->second, m_pChannelGroupSE, false, ppChannel);
   else {
       m_pSystem->playSound(iter->second, m_pChannelGroupSE, false, &m_pChannel);
}
if (bLoop)
       {
           if (nullptr != ppChannel)
           (*ppChannel)->setMode(FMOD_LOOP_NORMAL);
           else
               m_pChannel->setMode(FMOD_LOOP_NORMAL);
   }
   
if (nullptr != ppChannel)
      (*ppChannel)->setVolume(fVolume);
   else
       m_pChannel->setVolume(fVolume);
   
 m_pSystem->update();
}

void CSound::PlayBGM(FMOD::Channel** ppChannel, _tchar* pSoundKey, _float fVolume)
{
   map<TCHAR*, Sound*>::iterator iter;
   
   iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->_bool
   	{
   		return !lstrcmp(pSoundKey, iter.first);
   	});
   
   if (iter == m_mapSound.end())
   	return;
   FMOD::Channel* m_pChannel = nullptr;
   
   if (nullptr != ppChannel)
       m_pSystem->playSound(iter->second, m_pChannelGroupBGM, false, ppChannel);
   else
       m_pSystem->playSound(iter->second, m_pChannelGroupBGM, false, &m_pChannel);
   
   if (nullptr != ppChannel)
   {
       (*ppChannel)->setMode(FMOD_LOOP_NORMAL);
       (*ppChannel)->setVolume(fVolume);
   }
   else {
   
       m_pChannel->setMode(FMOD_LOOP_NORMAL);
       m_pChannel->setVolume(fVolume);
   }
}

void CSound::StopSound(FMOD::Channel** ppChannel)
{
    (*ppChannel)->stop();
}

void CSound::StopAll()
{
    m_pChannelGroupBGM->stop();
    m_pChannelGroupSE->stop();
}

void CSound::SetChannelVolume(FMOD::Channel** ppChannel, _float fDis, _vector vLength)
{
    _float fLength = XMVectorGetX(XMVector3Length(vLength)); 
		
	_float fVolume = (fLength > fDis) ? 0.f : (fDis - fLength);

	(*ppChannel)->setVolume(fVolume * 0.01f);
	m_pSystem->update();
}

void CSound::LoadSoundFile(const _char* soundFile)
{
    WRITE_LOCK;
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

    m_pChannelGroupSE->release();
	m_pChannelGroupBGM->release();
	m_pSystem->release();
}
