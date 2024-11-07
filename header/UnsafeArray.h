#ifndef _SM_UNSAFEARRAY
#define _SM_UNSAFEARRAY
#include <stdio.h>
#include <type_traits>

#define PRINT_STACK
#if defined(PRINT_STACK)
#include "IO/DebugTool.h"
#endif

template <typename T> struct UnsafeArray
{
private:
	T *p;

	void SetPtr(T *p)
	{
/*		if (p == 0)
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
		}*/
		this->p = p;
	}

public:
//private:
	UnsafeArray(T *p)
	{
		this->SetPtr(p);
	}

public:
	UnsafeArray() = default;
	UnsafeArray(std::nullptr_t) = delete;

	template<typename V> UnsafeArray(UnsafeArray<V> p)
	{
		this->p = p.Ptr();
	}

	Bool Set(T *p)
	{
		if (p == 0)
			return false;
		this->p = p;
		return true;
	}

	T* operator->() const
	{
        return this->p;
    }

	Bool operator<(const UnsafeArray<T> p)
	{
		return this->p < p.p;
	}

	Bool operator>(const UnsafeArray<T> p)
	{
		return this->p > p.p;
	}

	Bool operator<=(const UnsafeArray<T> p)
	{
		return this->p <= p.p;
	}

	Bool operator>=(const UnsafeArray<T> p)
	{
		return this->p >= p.p;
	}

	Bool operator==(const UnsafeArray<T> ptr)
	{
		return this->p == ptr.p;
	}

	Bool operator!=(const UnsafeArray<T> ptr)
	{
		return this->p != ptr.p;
	}

	UnsafeArray<T> operator++(int)
	{
		UnsafeArray<T> tmp = *this;
		this->p++;
		return tmp;
	}

	UnsafeArray<T> operator++()
	{
		this->p++;
		return *this;
	}

	UnsafeArray<T> operator--(int)
	{
		UnsafeArray<T> tmp = this->p;
		this->p--;
		return tmp;
	}

	UnsafeArray<T> operator--()
	{
		this->p--;
		return *this;
	}

	T &operator[] (UOSInt index) const
	{
		return this->p[index];
	}

	T &operator[] (OSInt index) const
	{
		return this->p[index];
	}

	UnsafeArray<T> operator+(OSInt val) const
	{
		UnsafeArray<T> ret = *this;
		ret += val;
		return ret;
	}

	UnsafeArray<T> operator+(UOSInt val) const
	{
		UnsafeArray<T> ret = *this;
		ret += val;
		return ret;
	}

	UnsafeArray<T> operator-(OSInt val) const
	{
		UnsafeArray<T> ret = *this;
		ret -= val;
		return ret;
	}

	UnsafeArray<T> operator-(UOSInt val) const
	{
		UnsafeArray<T> ret = *this;
		ret -= val;
		return ret;
	}


	UnsafeArray<T> &operator+=(OSInt val)
	{
		this->p += val;
		return *this;
	}

	UnsafeArray<T> &operator+=(UOSInt val)
	{
		this->p += val;
		return *this;
	}

	UnsafeArray<T> &operator-=(OSInt val)
	{
		this->p -= val;
		return *this;
	}

	UnsafeArray<T> &operator-=(UOSInt val)
	{
		this->p -= val;
		return *this;
	}

#if _OSINT_SIZE == 64
	T &operator[] (UInt32 index) const
	{
		return this->p[index];
	}

	T &operator[] (Int32 index) const
	{
		return this->p[index];
	}

	UnsafeArray<T> operator+(Int32 val) const
	{
		UnsafeArray<T> ret = *this;
		ret += val;
		return ret;
	}

	UnsafeArray<T> operator+(UInt32 val) const
	{
		UnsafeArray<T> ret = *this;
		ret += val;
		return ret;
	}

	UnsafeArray<T> operator-(Int32 val) const
	{
		UnsafeArray<T> ret = *this;
		ret -= val;
		return ret;
	}

	UnsafeArray<T> operator-(UInt32 val) const
	{
		UnsafeArray<T> ret = *this;
		ret -= val;
		return ret;
	}

	UnsafeArray<T> &operator+=(Int32 val)
	{
		this->p += val;
		return *this;
	}

	UnsafeArray<T> &operator+=(UInt32 val)
	{
		this->p += val;
		return *this;
	}

	UnsafeArray<T> &operator-=(Int32 val)
	{
		this->p -= val;
		return *this;
	}

	UnsafeArray<T> &operator-=(UInt32 val)
	{
		this->p -= val;
		return *this;
	}
#endif

	OSInt operator-(UnsafeArray<T> p)
	{
		return this->p - p.p;
	}

	OSInt operator-(T *p)
	{
		return this->p - p;
	}

	T &operator*() const
	{
		return this->p[0];
	}

	T *Ptr() const
	{
		return this->p;
	}

	void CopyFromNO(UnsafeArray<const T> srcArr, UOSInt arrSize)
	{
		MemCopyNO(this->p, srcArr.Ptr(), arrSize * sizeof(T));
	}

	template <typename V> static UnsafeArray<T> ConvertFrom(UnsafeArray<V> ptr)
	{
		UnsafeArray<T> ret;
		ret.p = (T*)ptr.Ptr();
		return ret;
	}

	static UnsafeArray<T> FromPtr(T *ptr)
	{
		return UnsafeArray<T>(ptr);
	}

	static UnsafeArray<T> FromPtrNoCheck(T *ptr)
	{
		UnsafeArray<T> ret;
		ret.p = ptr;
		return ret;
	}
};

template<typename V> OSInt operator-(V *p1, UnsafeArray<V> p2)
{
	return p1 - p2.Ptr();
}

template<typename V> Bool operator>(V *p1, UnsafeArray<V> p2)
{
	return p1 > p2.Ptr();
}

template<typename V> Bool operator<(V *p1, UnsafeArray<V> p2)
{
	return p1 < p2.Ptr();
}

template<typename V> Bool operator>=(V *p1, UnsafeArray<V> p2)
{
	return p1 >= p2.Ptr();
}

template<typename V> Bool operator<=(V *p1, UnsafeArray<V> p2)
{
	return p1 <= p2.Ptr();
}
#define UARR(arr) UnsafeArray<std::remove_reference<decltype( arr[0] )>::type>::FromPtrNoCheck(arr)
#endif
