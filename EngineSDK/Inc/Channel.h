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
                                     _float fCurrentPosition, _bool bMotionChanging, _float fTransTime);
	void init_changeTime() { m_fChangeTime = 0; }

    private:
	_char								m_szName[MAX_PATH];
	_uint								m_iNumKeyFrames = {};
	vector<KEYFRAME>					m_KeyFrames;
	_uint								m_iBoneIndex = {};
	_float							    m_fChangeTime = { 0.f };
	_vector								m_vLastScale = {};
	_vector								m_vLastRotation = {};
	_vector								m_vLastPosition = {};

public:
	static CChannel* Create(HANDLE& hFile);
	virtual void Free() override;
};
END
