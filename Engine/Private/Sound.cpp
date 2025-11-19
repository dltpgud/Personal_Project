
#include "Sound.h"
#include "GameInstance.h"
#include "Transform.h"
CSound::CSound() : m_pSystem{nullptr}, m_pGameInstance{CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CSound::Initialize()
{
	System_Create(&m_pSystem, FMOD_VERSION);
    m_pSystem->init(512, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, NULL);
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

 m_pSystem->playSound(iter->second, m_pChannelGroupSE, false, ppChannel);

 if (bLoop)
 {
   (*ppChannel)->setMode(FMOD_LOOP_NORMAL);
 }

  (*ppChannel)->setVolume(fVolume);
   
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
   
       m_pSystem->playSound(iter->second, m_pChannelGroupBGM, false, ppChannel);

       (*ppChannel)->setMode(FMOD_LOOP_NORMAL);
       (*ppChannel)->setVolume(fVolume);
   
}

void CSound::StopAll()
{
    m_pChannelGroupBGM->stop();
    m_pChannelGroupSE->stop();
}

void CSound::Set3DListenerAttributes()
{
    if (m_pGameInstance->Get_Player() == nullptr)
        return;

    _float4 CamLook =  *m_pGameInstance->Get_CamLook();
    _float4 CamUp = *m_pGameInstance->Get_CamUp();
    _vector PlayerPos =  m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_POSITION);
    _vector PlayerLook = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_LOOK);
    _vector PlayerUp = m_pGameInstance->Get_Player()->Get_Transform()->Get_TRANSFORM(CTransform::T_UP);
    _vector vVel = m_pGameInstance->Get_Player()->Get_Transform()->Get_Velocity();

    FMOD_VECTOR listenerVel = {XMVectorGetX(vVel), XMVectorGetY(vVel), XMVectorGetZ(vVel)};
    FMOD_VECTOR listenerPos = {XMVectorGetX(PlayerPos), XMVectorGetY(PlayerPos), XMVectorGetZ(PlayerPos)};
    FMOD_VECTOR listenerForward = {XMVectorGetX(PlayerLook), XMVectorGetY(PlayerLook), XMVectorGetZ(PlayerLook)};
    FMOD_VECTOR listenerUp = {XMVectorGetX(PlayerUp), XMVectorGetY(PlayerUp), XMVectorGetZ(PlayerUp)};

   m_pSystem->set3DListenerAttributes(0, &listenerPos, &listenerVel, &listenerForward, &listenerUp);

   m_pSystem->update();
}

void CSound::LoadSoundFile(const _char* soundFile,_bool isBGM)
{
    //WRITE_LOCK;
    unique_lock<mutex> lock(m_mutex);
    
	_char szCurPath[128] = "../../Client/Bin/Resources/Sound/";	 // 상대 경로
	_char szFullPath[128] = "";

	strcpy_s(szFullPath, szCurPath);

	_char szfull[128]{};

	strcpy_s(szfull, soundFile);

	strcat_s(szFullPath, szfull);

	Sound* pSound = nullptr;
        FMOD_RESULT eRes{};


    if (isBGM)
        eRes= m_pSystem->createSound(szFullPath, FMOD_2D, 0, &pSound);
    else
    {
        eRes = m_pSystem->createSound(szFullPath, FMOD_3D | FMOD_3D_LINEARROLLOFF, 0, &pSound);
        pSound->set3DMinMaxDistance(1.f, 80.0f); // 1m ~ 80m 까지 들림
    }

    if (eRes == FMOD_OK)
	{
		_int iLength = (_int)(strlen(szfull) + 1);

		TCHAR* pSoundKey = new TCHAR[iLength];
		ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

		MultiByteToWideChar(CP_ACP, 0, szfull, iLength, pSoundKey, iLength);

		m_mapSound.emplace(pSoundKey, pSound);
	}

	m_pSystem->update();
}
void CSound::UpdateSoundPosition(FMOD::Channel* pChannel, CTransform* pTransform)
{
   _vector pos = pTransform->Get_TRANSFORM(CTransform::T_POSITION);
   _vector vel = pTransform->Get_Velocity();

   FMOD_VECTOR vPos = {XMVectorGetX(pos), XMVectorGetY(pos), XMVectorGetZ(pos)};
   FMOD_VECTOR vVel = {XMVectorGetX(vel), XMVectorGetY(vel), XMVectorGetZ(vel)};

   pChannel->set3DAttributes(&vPos, &vVel);
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
    Safe_Release(m_pGameInstance);
}
