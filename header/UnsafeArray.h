#ifndef _SM_UNSAFEARRAY
#define _SM_UNSAFEARRAY
#include "MyMemory.h"
#include <stdio.h>

template <typename T> struct UnsafeArray
{
private:
	T *p;
private:
	void SetPtr(T *p)
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
		}
		this->p = p;
	}

public:
	UnsafeArray() = default;

	UnsafeArray(T *p)
	{
		this->SetPtr(p);
	}

	template<typename V> UnsafeArray(UnsafeArray<V> p)
	{
		this->p = p.Ptr();
	}

	T *Ptr() const
	{
		return this->p;
	}

	Bool operator==(UnsafeArray<T> ptr)
	{
		return this->p == ptr.p;
	}

	UnsafeArray<T> operator++(int)
	{
		T *tmp = this->p;
		this->p++;
		return tmp;
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
		return UnsafeArray<T>(this->p + val);
	}

	UnsafeArray<T> operator+(UOSInt val) const
	{
		return UnsafeArray<T>(this->p + val);
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
		return UnsafeArray<T>(this->p + val);
	}

	UnsafeArray<T> operator+(UInt32 val) const
	{
		return UnsafeArray<T>(this->p + val);
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
#endif

	OSInt operator-(UnsafeArray<T> p)
	{
		return this->p - p.p;
	}

	T &operator*() const
	{
		return this->p[0];
	}

	void CopyFromNO(UnsafeArray<T> srcArr, UOSInt arrSize)
	{
		MemCopyNO(this->p, srcArr.p, arrSize * sizeof(T));
	}

	template <typename V> static UnsafeArray<T> ConvertFrom(UnsafeArray<V> ptr)
	{
		UnsafeArray<T> ret;
		ret.p = (T*)ptr.Ptr();
		return ret;
	}
};

template<typename V> OSInt operator-(V *p1, UnsafeArray<V> p2)
{
	return p1 - p2.Ptr();
}
#endif
