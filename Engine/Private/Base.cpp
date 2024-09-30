#include "..\Public\Base.h"

CBase::CBase()
{
}

_uint CBase::AddRef()
{
	return ++m_iRefCnt;
}

_uint CBase::Release()
{
	if (0 == m_iRefCnt)
	{
		Free();  // 소멸자를 디폴트로 놓았기에 여기서 호출, 그 이유는 소멸자에서 자식소멸자, 부모 소멸자가 호출되는 구조가 문제될 수 있음으로..

		delete this;
		
		return 0;
	}

	else
		return m_iRefCnt--;
}

void CBase::Free()
{

}
