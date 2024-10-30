#pragma once
#include "Base.h"
BEGIN(Engine)


class CChannel :
	public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(HANDLE& hFile);
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex,
		_float fCurrentPosition, _bool bMotionChanging, _float fTransTime, KEYFRAME tLasKey);
	KEYFRAME Init_LastKeyFrame(const vector<class CBone*>& Bones);

private:
	_char								m_szName[MAX_PATH];
	_uint								m_iNumKeyFrames = {};
	vector<KEYFRAME>					m_KeyFrames;
	_int								m_iBoneIndex = {};
	/*

	_float m_fChangeTime;
	_vector								m_vLastScale = {};
	_vector								m_vLastRotation = {};
	_vector								m_vLastPosition = {};
	*/
public:
	static CChannel* Create(HANDLE& hFile);
	virtual void Free() override;
};
END
