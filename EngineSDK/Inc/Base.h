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
 public :
	virtual void Free() ; // 소멸자 대체.. 순수 가상으로 놓아도 됨

};

template <typename T> class ENGINE_DLL TSharedPtr
{
public:
    TSharedPtr()
    {
    }
    TSharedPtr(T* ptr)
    {
        Set(ptr);
    }

    // 복사
    TSharedPtr(const TSharedPtr& rhs)
    {
        Set(rhs._ptr);
    }

    // 이동
    TSharedPtr(TSharedPtr&& rhs)
    {
        _ptr = rhs._ptr;
        rhs._ptr = nullptr;
    }

    // 상속 관계 복사
    template <typename U> TSharedPtr(const TSharedPtr<U>& rhs)
    {
        Set(static_cast<T*>(rhs._ptr));
    }

    ~TSharedPtr()
    {
        Release();
    }

public:
    // 복사 연산자
    TSharedPtr& operator=(const TSharedPtr& rhs)
    {
        if (_ptr != rhs._ptr)
        {
            Release();
            Set(rhs._ptr);
        }
        return *this;
    }

    // 이동 연산자
    TSharedPtr& operator=(TSharedPtr&& rhs)
    {
        Release();
        _ptr = rhs._ptr;
        rhs._ptr = nullptr;
        return *this;
    }

    bool operator==(const TSharedPtr& rhs) const
    {
        return _ptr == rhs._ptr;
    }
    bool operator==(T* ptr) const
    {
        return _ptr == ptr;
    }
    bool operator!=(const TSharedPtr& rhs) const
    {
        return _ptr != rhs._ptr;
    }
    bool operator!=(T* ptr) const
    {
        return _ptr != ptr;
    }
    bool operator<(const TSharedPtr& rhs) const
    {
        return _ptr < rhs._ptr;
    }
    T* operator*()
    {
        return _ptr;
    }
    const T* operator*() const
    {
        return _ptr;
    }
    operator T*() const
    {
        return _ptr;
    }
    T* operator->()
    {
        return _ptr;
    }
    const T* operator->() const
    {
        return _ptr;
    }

    bool IsNull()
    {
        return _ptr == nullptr;
    }

private:
    inline void Set(T* ptr)
    {
        _ptr = ptr;
        if (ptr)
            ptr->AddRef();
    }

    inline void Release()
    {
        if (_ptr != nullptr)
        {
            _ptr->Release();
            _ptr = nullptr;
        }
    }

private:
    T* _ptr = nullptr;
};


END