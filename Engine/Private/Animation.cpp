#include "Animation.h"
#include "Channel.h"


CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& Proto)
    : m_fDuration{ Proto.m_fDuration }, m_fTickPerSecond{ Proto.m_fTickPerSecond }, m_iNumChannels{ Proto.m_iNumChannels },
      m_Channels{Proto.m_Channels}, m_iChannelKeyFrameIndices{Proto.m_iChannelKeyFrameIndices}, m_fCurrentPosition{Proto.m_fCurrentPosition}
{
    strcpy_s(m_szName, Proto.m_szName);

    for (auto& pChannel : m_Channels) Safe_AddRef(pChannel);

    m_vLastKeyFrame.resize(m_iNumChannels);
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
    m_vLastKeyFrame.resize(m_iNumChannels);

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
    // 콜백함수 실행
    Run_CallbackFunc(round(m_fCurrentPosition));

    // 애니메이션 전환 중 이면 보션 전환 중 시간을 증가시키고
    if (m_fChangingTime < m_fMotionChangingTIme)
        m_fChangingTime += m_fTickPerSecond * fTimeDelta;
    else // 전환 중이 아니라면 현재 위치를 증가 시킴
        m_fCurrentPosition += m_fTickPerSecond * fTimeDelta;

    // 애니메이션이 끝까지 재생되면
    if (m_fCurrentPosition >= m_fDuration)
    {
        if (isLoop) // 반복하는 애니메이션이라면
        {
            m_fCurrentPosition = 0.f;
            m_CallbackCheck.clear();
        }
        else if (false == isLoop)
        {
            m_CallbackCheck.clear();
            // 한 번만 재생하는 애니메이션이라면
            return true;
        }
    }

    _float fChannelAnimTime{};
    // 실제 보간을 위한 시간을 애니메이션 전환 중인 여부 판단으로 넘김
    if (m_fChangingTime < m_fMotionChangingTIme)
        fChannelAnimTime = m_fChangingTime;
    else
        fChannelAnimTime = m_fCurrentPosition;

    // 각 채널에 대해 변환 행렬을 업데이트
    for (_uint i = 0; i < m_iNumChannels; i++)
        m_Channels[i]->Update_TransformationMatrix(Bones, &m_iChannelKeyFrameIndices[i], fChannelAnimTime,
                                                   m_fChangingTime < m_fMotionChangingTIme, m_fMotionChangingTIme,
                                                   m_vLastKeyFrame[i]);
    return false;
}

void CAnimation::init_Loop(const vector<class CBone*>& Bones)
{
    m_fCurrentPosition = 0;
    m_fChangingTime = 0.f;

    for (_uint i = 0; i < m_iNumChannels; i++)
    {      
        m_iChannelKeyFrameIndices[i] = 0;
        m_vLastKeyFrame[i] = m_Channels[i]->Init_LastKeyFrame(Bones);
        m_vLastKeyFrame[i].fTrackPosition = m_fMotionChangingTIme;  

    }
    m_CallbackCheck.clear();
}

void CAnimation::Callback(_int Duration, function<void()> func)
{
  
    m_CallbackFunc[Duration].emplace_back(func);
}

void CAnimation::Run_CallbackFunc(_int Duration)
{
    if (m_CallbackCheck.find(Duration) != m_CallbackCheck.end())
        return;
    auto iter = m_CallbackFunc.find(Duration);
    if (iter == m_CallbackFunc.end())
        return;

    for (auto& callback : iter->second) { callback(); }

    m_CallbackCheck.insert(Duration);
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
