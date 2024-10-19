#include "Animation.h"
#include "Channel.h"
CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& Proto)
    : m_fDuration{ Proto.m_fDuration }, m_fTickPerSecond{ Proto.m_fTickPerSecond }, m_iNumChannels{ Proto.m_iNumChannels },
    m_Channels{ Proto.m_Channels }, m_iChannelKeyFrameIndices{ Proto.m_iChannelKeyFrameIndices }
{
    strcpy_s(m_szName, Proto.m_szName);

    for (auto& pChannel : m_Channels) Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(HANDLE& hFile)
{
    DWORD dwByte{};
    _bool bReadFile{};

    bReadFile = ReadFile(hFile, m_szName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
    bReadFile = ReadFile(hFile, &m_fDuration, sizeof(_float), &dwByte, nullptr);
    bReadFile = ReadFile(hFile, &m_fTickPerSecond, sizeof(_float), &dwByte, nullptr);
    bReadFile = ReadFile(hFile, &m_iNumChannels, sizeof(m_iNumChannels), &dwByte, nullptr);

    m_iChannelKeyFrameIndices.resize(m_iNumChannels);

    for (_uint i = 0; i < m_iNumChannels; i++)
    {
        CChannel* pChannel = CChannel::Create(hFile);

        if (!pChannel)
        {
            MSG_BOX("Failed to Load Channel");
            return E_FAIL;
        }

        m_Channels.push_back(pChannel);
    }

    return S_OK;
}

_bool CAnimation::Update_TransformationMatrix(const vector<class CBone*>& Bones, _bool isLoop, _float fTimeDelta)
{
    _float fChannelAnimTime{};


    if (m_fChangingTime < m_fMotionChangingTIme)
        m_fChangingTime += m_fTickPerSecond * fTimeDelta;
    else
        m_fCurrentPosition += m_fTickPerSecond * fTimeDelta;

    if (m_fCurrentPosition >= m_fDuration)
    {
        if (isLoop)
            m_fCurrentPosition = 0.f;
        else if (false == isLoop)
        {

            return true;
        }
    }

    if (m_fChangingTime < m_fMotionChangingTIme)
        fChannelAnimTime = m_fChangingTime;
    else
        fChannelAnimTime = m_fCurrentPosition;

    for (_uint i = 0; i < m_iNumChannels; i++)
        m_Channels[i]->Update_TransformationMatrix(Bones, &m_iChannelKeyFrameIndices[i], fChannelAnimTime,
            m_fChangingTime < m_fMotionChangingTIme, m_fMotionChangingTIme);

    return false;
}

void CAnimation::init_Loop()
{
    m_fCurrentPosition = 0;
    m_fChangingTime = 0.f;

    for (_uint i = 0; i < m_iNumChannels; i++)
    {
        m_iChannelKeyFrameIndices[i] = 0;
        m_Channels[i]->init_changeTime();
    }
}

CAnimation* CAnimation::Create(HANDLE& hFile)
{
    CAnimation* pInstance = new CAnimation();

    if (FAILED(pInstance->Initialize(hFile)))
    {
        MSG_BOX("Failed To Created : CAnimation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CAnimation* CAnimation::Clone()
{
    return new CAnimation(*this);
}

void CAnimation::Free()
{
    __super::Free();

    for (auto& pChannel : m_Channels) Safe_Release(pChannel);
    m_Channels.clear();
}
