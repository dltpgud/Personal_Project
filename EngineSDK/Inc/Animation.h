#pragma once
#include "Base.h"

BEGIN(Engine)

class CAnimation : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& Proto);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(HANDLE& hFile);
	_bool Update_TransformationMatrix(const vector<class CBone*>& Bones, _bool isLoop, _float fTimeDelta);
	void init_Loop(const vector<class CBone*>& Bones);
    void Callback(_int Duration, function<void()> func);
    void Run_CallbackFunc(_int Duration);


private:
	_char					m_szName[MAX_PATH] = {};
    _float					m_fDuration = {0.f}; // 애니메이션의 총 클립길이
	_float					m_fTickPerSecond = { 0.f }; // 애니메이션 프레임 속도 

	_float					m_fChangingTime{4.f}; // 전환중 경과시간
	_float				    m_fMotionChangingTIme{ 3.f }; //전환에 걸리는 총 시간


	_float					m_LastCurrentPosition = { 0.f };// 마지막 진행 위치
	_uint					m_iNumChannels = {}; // 애니메이션 채널(뼈) 개수
	vector<class CChannel*>	m_Channels; // 채널 저장 컨테이너
        vector<_uint>       m_iChannelKeyFrameIndices; // 각 채널의 현재 키프레임 인덱스

	vector<KEYFRAME>        m_vLastKeyFrame; // 마지막 키프레임 

     _float m_fCurrentPosition = {0.f};// 현재 애니메이션 진행 위치
     unordered_map < _int, vector < function<void()>>> m_CallbackFunc;
     unordered_set<_int> m_CallbackCheck;

 public:
	static CAnimation* Create(HANDLE& hFile);
	CAnimation* Clone();
	virtual void Free() override;
};

END