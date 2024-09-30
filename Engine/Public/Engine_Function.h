﻿#ifndef Engine_Function_h__
#define Engine_Function_h__

namespace Engine
{
	// 템플릿은 기능의 정해져있으나 자료형은 정해져있지 않은 것
	// 기능을 인스턴스화 하기 위하여 만들어두는 틀

	template<typename T>
	void	Safe_Delete(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	void	Safe_Delete_Array(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete[] Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	unsigned int Safe_Release(T& pInstance)
	{
		unsigned int		dwRefCnt = { 0 };

 		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release();

			if (0 == dwRefCnt)
				pInstance = nullptr;
		}

		return dwRefCnt;
	}

	template<typename T>
	unsigned int Safe_AddRef(T& pInstance)
	{
		unsigned int		dwRefCnt = { 0 };

		if (nullptr != pInstance)		
			dwRefCnt = pInstance->AddRef();	

		return dwRefCnt;
	}


	//// Functor ////정쌤은 이부분을 지우심//
	class CTag_Finder
	{
	public:
		explicit CTag_Finder(const _wstring* pTag) : m_pTargetTag(pTag) {}
		~CTag_Finder(void) {}

	public:
		template<typename T>
		_bool		operator()(const T& pair)
		{
			if (0 == lstrcmpW(m_pTargetTag, pair.first))
				return true;

			return false;
		}

	private:
		const _wstring* m_pTargetTag = nullptr;
	};

	// 릴리즈 호출
	class CDeleteObj
	{
	public:
		explicit CDeleteObj(void) {}
		~CDeleteObj(void) {}
	public: // operator
		template <typename T>
		void operator () (T& pInstance)
		{
			_ulong dwRefCnt = 0;

			dwRefCnt = pInstance->Release();

			if (0 == dwRefCnt)
				pInstance = nullptr;
		}
	};

	// 연관컨테이너 삭제용
	class CDeleteMap
	{
	public:
		explicit CDeleteMap(void) {}
		~CDeleteMap(void) {}
	public: // operator	
		template <typename T>
		void operator () (T& Pair)
		{
			_ulong dwRefCnt = 0;

			dwRefCnt = Pair.second->Release();

			if (0 == dwRefCnt)
				Pair.second = NULL;
		}
	};

	static DWORD FtoDw(_float f) { return *((DWORD*)&f); }

}
#endif // Engine_Function_h__
