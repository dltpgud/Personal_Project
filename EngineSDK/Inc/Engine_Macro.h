#ifndef Engine_Macro_h__
#define Engine_Macro_h__


namespace Engine
{
#ifndef			MSG_BOX
#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
#endif

#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			END						}

#define			USING(NAMESPACE)		using namespace NAMESPACE;

#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif


#define NULL_CHECK( _ptr)	\
		{if( _ptr == 0){__asm { int 3 };return;}}

#define NULL_CHECK_RETURN( _ptr, _return)	\
		{if( _ptr == 0){__asm { int 3 };return _return;}}

#define NULL_CHECK_MSG( _ptr, _message )		\
		{if( _ptr == 0){MessageBox(NULL, _message, L"System Message",MB_OK);__asm { int 3 };}}

#define NULL_CHECK_RETURN_MSG( _ptr, _return, _message )	\
		{if( _ptr == 0){MessageBox(NULL, _message, L"System Message",MB_OK);__asm { int 3 };return _return;}}

#define FAILED_CHECK(_hr)	if( ((HRESULT)(_hr)) < 0 )	\
		{ MessageBoxW(NULL, L"Failed", L"System Error",MB_OK); __asm { int 3 }; return E_FAIL;}

#define FAILED_CHECK_RETURN(_hr, _return)	if( ((HRESULT)(_hr)) < 0 )		\
		{ MessageBoxW(NULL, L"Failed", L"System Error",MB_OK); __asm { int 3 }; return _return;}

#define FAILED_CHECK_MSG( _hr, _message)	if( ((HRESULT)(_hr)) < 0 )	\
		{ MessageBoxW(NULL, _message, L"System Message",MB_OK); __asm { int 3 };return E_FAIL;}

#define FAILED_CHECK_RETURN_MSG( _hr, _return, _message)	if( ((HRESULT)(_hr)) < 0 )	\
		{ MessageBoxW(NULL, _message, L"System Message",MB_OK); __asm { int 3 };return _return;}


#define USE_MANY_LOCKS(count) CSpinLock m_locks[count];
#define USE_LOCK USE_MANY_LOCKS(1)
#define READ_LOCK_IDX(idx) CReadLockGuard readLockGuard_##idx(m_locks[idx], typeid(this).name());
#define READ_LOCK READ_LOCK_IDX(0)
#define WRITE_LOCK_IDX(idx) CWriteLockGuard WriteLockGuard_##idx(m_locks[idx], typeid(this).name());
#define WRITE_LOCK WRITE_LOCK_IDX(0)

#define CRASH(cause)                                                                                                   \
    {                                                                                                                  \
        _uint32* crash = nullptr;                                                                                       \
        __analysis_assume(crash != nullptr);                                                                           \
        *crash = 0xDEADBEEF;                                                                                           \
    }

#define NO_COPY(CLASSNAME)								\
			private:										\
			CLASSNAME(const CLASSNAME&) = delete;					\
			CLASSNAME& operator = (const CLASSNAME&) = delete;		

#define DECLARE_SINGLETON(CLASSNAME)					\
			NO_COPY(CLASSNAME)								\
			private:										\
			static CLASSNAME*	m_pInstance;				\
			public:											\
			static CLASSNAME*	GetInstance( void );		\
			static unsigned int DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)								\
			CLASSNAME*	CLASSNAME::m_pInstance = nullptr;			\
			CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
				if(nullptr == m_pInstance) {						\
					m_pInstance = new CLASSNAME;					\
				}													\
				return m_pInstance;									\
			}														\
			unsigned int CLASSNAME::DestroyInstance( void ) {		\
				unsigned int iRefCnt = { 0 };						\
				if(nullptr != m_pInstance)	{						\
					iRefCnt = m_pInstance->Release();				\
					if(0 == iRefCnt)								\
						m_pInstance = nullptr;						\
				}													\
				return iRefCnt;										\
			}
}

#endif // Engine_Macro_h__
