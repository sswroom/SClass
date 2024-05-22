#ifndef _SM_UNSAFEARRAY
#define _SM_UNSAFEARRAY
#include "UnsafeArrayOpt.h"
#include <stdio.h>

#define PRINT_STACK
#if defined(PRINT_STACK)
#include "IO/DebugTool.h"
#endif

template <typename T> struct UnsafeArray : public UnsafeArrayOpt<T>
{
private:
	virtual void SetPtr(T *p)
	{
		if (p == 0)
		{
#if defined(__PRETTY_FUNCTION__)
			printf("Null found in UnsafeArray: " __PRETTY_FUNCTION__ "\r\n");
#elif defined(__FUNCSIG__)
			printf("Null found in UnsafeArray: " __FUNCSIG__ "\r\n");
#elif defined(__FUNCTION__)
			printf("Null found in UnsafeArray: " __FUNCTION__ "\r\n");
#elif defined(__func__)
			printf("Null found in UnsafeArray: " __func__ "\r\n");
#else
			printf("Null found in UnsafeArray\r\n");
#endif
#if defined(PRINT_STACK)
			IO::DebugTool::PrintStackTrace();
#endif
		}
		this->p = p;
	}

public:
	UnsafeArray() = default;
	UnsafeArray(std::nullptr_t) = delete;
	UnsafeArray(T *p)
	{
		this->SetPtr(p);
	}

	template<typename V> UnsafeArray(UnsafeArray<V> p)
	{
		this->p = p.Ptr();
	}

	Bool Set(UnsafeArrayOpt<T> p)
	{
		if (p.Ptr() == 0)
			return false;
		this->p = p.Ptr();
		return true;
	}

	template <typename V> static UnsafeArray<T> ConvertFrom(UnsafeArray<V> ptr)
	{
		UnsafeArray<T> ret;
		ret.p = (T*)ptr.Ptr();
		return ret;
	}

	template <typename V> static UnsafeArray<T> FromOpt(UnsafeArrayOpt<V> p)
	{
		UnsafeArray<T> ret;
		ret.SetPtr(p.Ptr());
		return ret;
	}
};
#endif
