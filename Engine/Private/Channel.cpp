#include "Channel.h"
#include "Bone.h"
CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(HANDLE& hFile)
{
    DWORD dwByte{};
    _bool bReadFile{};

    bReadFile = ReadFile(hFile, m_szName, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
    bReadFile = ReadFile(hFile, &m_iBoneIndex, sizeof(m_iBoneIndex), &dwByte, nullptr);
    bReadFile = ReadFile(hFile, &m_iNumKeyFrames, sizeof(m_iNumKeyFrames), &dwByte, nullptr);

    KEYFRAME* pKeyFrame = new KEYFRAME;

    for (_uint i = 0; i < m_iNumKeyFrames; i++)
    {
        bReadFile = ReadFile(hFile, pKeyFrame, sizeof(KEYFRAME), &dwByte, nullptr);

        m_KeyFrames.push_back(*pKeyFrame);
    }

    Safe_Delete(pKeyFrame);

    return S_OK;
}

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex,
    _float fCurrentPosition, _bool bMotionChanging, _float fTransTime, KEYFRAME tLasKey)
{

    KEYFRAME LastKeyFrame = m_KeyFrames.back();

    _vector vScale;
    _vector vRotation;
    _vector vPosition;

    // 전환 중일 때
    if (bMotionChanging)
    {
        // 전환 비율을 계산 (전환 경과 시간에 따른 비율)
        _float fTransitionRatio = fCurrentPosition / tLasKey.fTrackPosition;

        _vector vCurrentDestScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
        _vector vCurrentDestRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);
        _vector vCurrentDestPosition = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vPosition), 1.f);

        vScale = XMVectorLerp(XMLoadFloat3(&tLasKey.vScale), vCurrentDestScale, fTransitionRatio);
        vRotation = XMQuaternionSlerp(XMLoadFloat4(&tLasKey.vRotation), vCurrentDestRotation, fTransitionRatio);
        vPosition = XMVectorLerp(XMLoadFloat3(&tLasKey.vPosition), vCurrentDestPosition, fTransitionRatio);

        _matrix TransformMatrix =
            XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

        Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformMatrix);

        return;
    }
    else if (fCurrentPosition >= LastKeyFrame.fTrackPosition)
    {
        vScale = XMLoadFloat3(&LastKeyFrame.vScale);
        vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
        vPosition = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f);
    }
    else
    {
        if (0.f == fCurrentPosition)
            *pCurrentKeyFrameIndex = 0;

        /*프레임 드랍시 키 프레임이 넓게 찍혀서 모델이 깨지는 현상을 막음*/
        while (fCurrentPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition) ++*pCurrentKeyFrameIndex;

        _float fLinearRatio = (fCurrentPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition) /
            (m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition -
                m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition);

        _vector vSourScale{}, vDestScale{};
        _vector vSourRotation{}, vDestRotation{};
        _vector vSourPosition{}, vDestPosition{};

        vSourScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
        vSourRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);
        vSourPosition = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vPosition), 1.f);

        vDestScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale);
        vDestRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation);
        vDestPosition = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vPosition), 1.f);

        vScale = XMVectorLerp(vSourScale, vDestScale, fLinearRatio); // 보간해주는 함수
        vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fLinearRatio);
        vPosition = XMVectorLerp(vSourPosition, vDestPosition, fLinearRatio);
    }

    _matrix TransformMatrix =
        XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformMatrix);
}

KEYFRAME CChannel::Init_LastKeyFrame(const vector<class CBone*>& Bones)
{
    KEYFRAME tKeyFrame{};

    _vector vReturn[3];

    XMMatrixDecompose(&vReturn[0], &vReturn[1], &vReturn[2],
        Bones[m_iBoneIndex]->Get_TransformationMatrix());

    XMStoreFloat3(&tKeyFrame.vScale, vReturn[0]);
    XMStoreFloat4(&tKeyFrame.vRotation, vReturn[1]);
    XMStoreFloat3(&tKeyFrame.vPosition, vReturn[2]);

    return tKeyFrame;
}

CChannel* CChannel::Create(HANDLE& hFile)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize(hFile)))
    {
        MSG_BOX("Failed To Created : CAnimation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CChannel::Free()
{
    __super::Free();
}
