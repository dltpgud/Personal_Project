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
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex, _float fCurrentPosition);

private:
	_char								m_szName[MAX_PATH];
	_uint								m_iNumKeyFrames = {};
	vector<KEYFRAME>					m_KeyFrames;
	_uint								m_iBoneIndex = {};

public:
	static CChannel* Create(HANDLE& hFile);
	virtual void Free() override;
};
END
