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
		

private:
	_char					m_szName[MAX_PATH] = {};
	_float					m_fDuration = { 0.f };
	_float					m_fTickPerSecond = { 0.f };
	_float					m_fCurrentPosition = { 0.f };


	_uint					m_iNumChannels = {};
	vector<class CChannel*>	m_Channels;
	vector<_uint>			m_iChannelKeyFrameIndices;
public:
	static CAnimation* Create(HANDLE& hFile);
	CAnimation* Clone();
	virtual void Free() override;
};

END