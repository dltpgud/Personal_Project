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
    _float					m_fDuration = {0.f}; // �ִϸ��̼��� �� Ŭ������
	_float					m_fTickPerSecond = { 0.f }; // �ִϸ��̼� ������ �ӵ� 

	_float					m_fChangingTime{4.f}; // ��ȯ�� ����ð�
	_float				    m_fMotionChangingTIme{ 3.f }; //��ȯ�� �ɸ��� �� �ð�


	_float					m_LastCurrentPosition = { 0.f };// ������ ���� ��ġ
	_uint					m_iNumChannels = {}; // �ִϸ��̼� ä��(��) ����
	vector<class CChannel*>	m_Channels; // ä�� ���� �����̳�
        vector<_uint>       m_iChannelKeyFrameIndices; // �� ä���� ���� Ű������ �ε���

	vector<KEYFRAME>        m_vLastKeyFrame; // ������ Ű������ 

     _float m_fCurrentPosition = {0.f};// ���� �ִϸ��̼� ���� ��ġ
     unordered_map < _int, vector < function<void()>>> m_CallbackFunc;
     unordered_set<_int> m_CallbackCheck;

 public:
	static CAnimation* Create(HANDLE& hFile);
	CAnimation* Clone();
	virtual void Free() override;
};

END