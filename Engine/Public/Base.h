#pragma once
/* 모든 클래스의 부모가 되는 클래스다. */
/* 레퍼런스 카운트를 관리한다. (AddRef, Release) */
#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;


public :
	/* 레퍼런스 카운트를 증가시킨다. 증가시킨 레퍼런스 카운트를 리턴한다. */
	_uint AddRef();

	/* (레퍼런스 카운트를 감소시킨다. or 삭제한다.) 감소시키기 전의 레퍼런스 카운트를 리턴한다. */
	_uint Release();

private:
	_uint			m_iRefCnt = { 0 };

public:
	virtual void Free() ; // 소멸자 대체.. 순수 가상으로 놓아도 됨

};

END